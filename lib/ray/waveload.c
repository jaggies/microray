/*
 * waveload.c
 *
 *  Created on: May 19, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // malloc
#include <sys/stat.h>
#include <math.h>
#include "os.h"
#include "vec2.h"
#include "vec3.h"
#include "world.h"
#include "waveload.h"
#include "waveutil.h"
#include "wavematl.h"

typedef enum { UNKNOWN = 0, BSPLINE = 1, BEZIER = 2 } CurveType;
typedef enum { NONE = 0, RATIONAL = 1 } CurveFlags;

bool sFourQuv = false; // Some wavefront files depend on this for proper texture placement

// For reading MTL library
typedef struct ShaderItem {
    char* name;
    Shader* shader;
} ShaderItem;

const int kMaxLine = 256; // 256 bytes per line

typedef struct Curve {
    CurveType   type;
    CurveFlags  flags;
    uint8_t     degree[2];
} Curve;

typedef struct State {
    Shader* shader; // current shader
    Curve curve;
    const char* filepath;
} State;

bool
loadWavefront(World* world, const char *filename, const char *options)
{
    bool result = false;
    State state;
    char *tmpstr = strdup(filename);
    FILE *infile = NULL;
    int linecount = 0;
    struct stat st;

    bzero(&state, sizeof(State));

    state.shader = (Shader*) createDefaultShader();
    addShader(world, "default", state.shader);

    state.filepath = dirname(tmpstr);

    if (stat(filename, &st)) {
        printf("Can't open %s\n", filename);
        return false;
    }

    // Parse Wavefront obj file
    if ( (infile = fopen(filename, "r")) ) {
        fprintf(stderr, "Loading Wavefront file '%s'\n", filename);
        do {
            char str[WAVE_MAX_LINE];
            const char* args[MAX_ARGS];
            int completed = 0;
            int progress = (int) (100.0f * ftell(infile) / st.st_size);

            linecount += waveGetLine(infile, str, WAVE_MAX_LINE);

            if (progress > completed) {
                fprintf(stderr, "\rLoading: %d%%", progress);
                fflush(stderr);
                completed = progress;
                if (progress == 100) {
                    fputc('\n', stderr);
                }
            }

            // strategy:
            //  g -> changes the state group path and starts a new mesh/surface.
            //  f -> creates a mesh if necessary and adds all subsequent meshes to it.
            //  usemtl -> checks for empty mesh and uses it or if not adds a new mesh object to group
            switch (str[0]) {
                case 'c':
                {
#if 0
                    size_t i;
                    size_t nargs = split(str, args, MAX_ARGS);
                    if (strcmp("cstype", args[0]) == 0) {
                        state.curve.type = UNKNOWN;
                        for (i = 1; i < nargs; i++) {
                            if (strcmp("bspline", args[i]) == 0) {
                                fprintf(stderr, "Starting new bspline\n");
                                state.curve.type = BSPLINE;
                                // TODO: create abstract curve storage and cleanup in "end"
                            } else if (strcmp("bezier", args[i]) == 0) {
                                fprintf(stderr, "Starting new bezier\n");
                                state.curve.type = BEZIER;
                            } else if (strcmp("rat", args[i]) == 0) {
                                state.curve.type = BSPLINE;
                                state.curve.flags |= RATIONAL;
                            } else {
                                fprintf(stderr, "unknown cstype parameter: '%s'\n", args[i]);
                            }
                        }
                    } else if (strcmp("curv2", args[0]) == 0) {
                        size_t i = 1;
                        while (i < nargs) {
                            int32_t value;
                            if (readint32(args[i++], &value)) {
                                // Negative number means read from end...
                                value = value < 0 ? state.nPoints + value : value - 1;
                                // TODO:
                                // state.curve.index.append(value);
                            }
                        }
                    } else {
                        warn("Unsupported: " << str << "\n");
                    }
#endif
                }
                break;

                case 'd':
                {
#if 0
                    if (strstr(str, "deg") == 0) {
                        size_t nargs = split(str, args, MAX_ARGS);
                        // Default to linear ("deg 1 1")
                        state.curve.degree[0] = nargs > 1 ? atoi(args[1]) : 1;
                        state.curve.degree[1] = nargs > 2 ? atoi(args[2]) : 1;
                    } else {
                        warn("Unsupported: " << str << "\n");
                    }
#endif
                }
                break;

                case 'e':
                {
#if 0
                    if (strstr(str, "end") == 0) {
                        const bool isCurve = state.curve.degree[Y] == -1;
                        if (isCurve) {
                            sptr<WaveObject> curve = new WaveCurve2d(state.membership, state.curve);
                            curve->shader = state.shader;
                            state.objects.append(curve.ptr());
                        } else {
                            sptr<WaveObject> surface = new WaveSurface(state.membership, state.curve);
                            surface->shader = state.shader;
                            state.objects.append(surface.ptr());
                        }
                        state.curve.dump();
                        state.curve = CommonCurveParams();
                    } else {
                        warn("Unsupported: " << str << "\n");
                    }
#endif
                }
                break;

                case 'f':
                {
                    Face* face = createFace();
                    readFace(str, face);
                    addFace(world, face); // TODO: Remove
                    generateTriangles(world, face, state.shader);
                }
                break;

                case 'o': // This may be wrong!
                case 'g':
                {
#if 0
                    // 'g' specifies group membership. For example, 'g a b c' means that everything
                    // that follows belongs to groups a, b and c.
                    state.membership = split(str.substr(1,str.size()));

                    // start a new object next time we see a face or surface
                    state.mesh = NULL;
#endif
                }
                break;

                case 'm':
                {
                    size_t nargs = split(str, args, MAX_ARGS);
                    if ( nargs > 1 && strcmp(args[0], "mtllib") == 0 ) {
                        loadMaterialLibrary(world, state.filepath, args[1]);
                    } else {
                        fprintf(stderr, "\rUnsupported: '%s'\n", str);
                    }
                }
                break;

                case 'p':
                {
#if 0
                    vector<string> items = split(str);
                    if (items[0] == "parm") {
                        List<double>* knots = NULL;
                        if (items[1] == "u") {
                            knots = &state.curve.uknots;
                        } else if (items[1] == "v") {
                            knots = &state.curve.vknots;
                        }
                        if (knots) {
                            for (size_t i = 2; i < items.size(); i++) {
                                double value;
                                if (get(items[i], value))
                                    knots->append(value);
                            }
                        }
                    } else {
                        warn("Unsupported: " << str << "\n");
                    }
#endif
                }
                break;

                case 's':
                {
#if 0
                    vector<string> items = split(str);
                    if (items[0] == "surf") {
                        assert(items.size() > 5);
                        size_t i = 1;
                        get(items[i++], state.curve.start[X]);
                        get(items[i++], state.curve.end[X]);
                        get(items[i++], state.curve.start[Y]);
                        get(items[i++], state.curve.end[Y]);
                        while (i < items.size()) {
                            int value;
                            if (get(items[i++], value)) {
                                // Negative number means read from end...
                                value = value < 0 ? state.points.size() + value : value - 1;
                                state.curve.index.append(value);
                            }
                        }
                    } else {
                        warn("Unsupported: " << str << "\n");
                    }
#endif
                }
                break;

                case 'u':
                {
                    size_t nargs = split(str, args, MAX_ARGS);
                    if (nargs == 2 && strcmp(args[0], "usemtl") == 0) {
                        state.shader = findShader(args[1], world);
                        if (!state.shader) {
                            fprintf(stderr, "\rFailed to find shader '%s'!\n", args[1]);
                            state.shader = findShader("default", world);
                        }
                    } else {
                        fprintf(stderr, "\rUnsupported: '%s'\n", str);
                    }
                }
                break;

                case 'v':
                    switch(str[1]) {
                        case 'n':
                        {
                            Vec3 normal;
                            addNormal(world, readvec3(str + 2, &normal));
                        }
                        break;

                        case 't':
                        {
                            Vec2 uv;
                            readvec2(str + 2, &uv);
                            if (sFourQuv) {
                                uv.x = (uv.x + 1.0f) * 0.5f;
                                uv.y = (uv.y + 1.0f) * 0.5f;
                            } else {
                                // Map all textures to the range [0..1].
                                uv.x = fmodf(uv.x, 1.0f);
                                uv.y = fmodf(uv.y, 1.0f);
                                uv.x = uv.x < 0.0f ? 1.0f + uv.x : uv.x;
                                uv.y = uv.y < 0.0f ? 1.0f + uv.y : uv.y;
                            }
                            addUv(world, &uv);
                        }
                        break;

                        case 'p':
                        {
#if 0
                            Vec2 point2d;
                            addCurvePoint2D(world, readvec2(str + 2, &point2d));
#endif
                        }
                        break;

                        case ' ':
                        case '\t':
                        {
                            Vec3 point;
                            addPoint(world, readvec3(str + 2, &point));
                        }
                        break;

                        default:
                            fprintf(stderr, "\rUnsupported vertex token '%s'\n", str);
                        break;
                    }
                break;

                case '#':
                    fprintf(stderr, "\r%s", str);   // echo comment to stderr
                break;

                case '\0':  // getline sometimes returns empty string
                case ' ':   // ignore whitespace
                case '\n':
                case '\r':  // MSDOS CR
                break;

                default:
                    fprintf(stderr, "\rUnsupported token '%s' at line %d\n", str , linecount);
                break;
            }
        } while (!feof(infile));
        fclose(infile);
        result = true;
    }

    free(tmpstr);
    fputc('\r', stderr);
    return result;
}

