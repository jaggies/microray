/*
 * wavematl.c
 *
 *  Created on: May 26, 2020
 *      Author: jmiller
 *
 * Loads an array with materials and names and returns them in alphabetical order
 * so they can be quickly searched.
 *
 * Supported material properties:
 *  Ns = PhongShader specular component. Ranges from 0 to 1000.
 *  Kd = Diffuse color weighted by the diffuse coefficient.
 *  Ka = Ambient color weighted by the ambient coefficient.
 *  Ks = Specular color weighted by the specular coefficient.
 *  d = Dissolve factor, 0-1, (pseudo-transparency). 0 is completely transparent, 1 is opaque.
 *  Ni = Refraction index. Values range from 1 upwards.
 *  illum = (0, 1, or 2):
 *        0 to disable lighting
 *        1 for ambient & diffuse only (specular color set to black)
 *        2 for full lighting (see below)
 *        3,4 ?
 *        sharpness = ?
 *  map_Kd = Diffuse color texture map.
 *  map_Ks = Specular color texture map.
 *  map_Ka = Ambient color texture map.
 *  map_Bump = Bump texture map.
 *  map_d = Opacity texture map.
 *  refl = reflection type and filename (?)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "world.h"
#include "wavematl.h"
#include "waveload.h"
#include "waveutil.h"

PhongShader* createDefaultShader() {
    Vec3 diffuse = { 0.5f, 0.5f, 0.5f };
    Vec3 specular = { 0.5f, 0.5f, 0.5f };
    Vec3 ambient = { 0.1f, 0.1f, 0.1f };
    float exp = 2.0f;
    float index = 1.0f;
    float reflect = 0.5f;
    float transmit = 0.0f;
    return createPhongShader(&diffuse, &specular, &ambient, exp, index, reflect, transmit);
}

bool loadMaterialLibrary(World* world, const char* path, const char* name)
{
    char* filename = fullpath(path, name);
    FILE *infile = fopen(filename, "r");

    if (infile) {
        int linecount = 1;  // FIXME: inaccurate if lines are escaped
        PhongShader* currentShader = NULL;

        const char* args[MAX_ARGS];
        do {
            char line[WAVE_MAX_LINE];
            linecount += waveGetLine(infile, line, sizeof(line));
            if (strlen(line) > 1) {
                int nargs = 0;
                switch (line[0]) {
                    case 'n': // newmtl
                        nargs = split(line, args, MAX_ARGS);
                        if (nargs > 0 && strcmp(args[0], "newmtl") == 0) {
                            currentShader = createDefaultShader();
                            char* name = strdup(nargs > 1 && strlen(args[1]) ? args[1] : "NoName");
                            addShader(world, name, (Shader*) currentShader);
                        } else {
                            fprintf(stderr, "Unknown token %s\n", args[0]);
                        }
                    break;

                    case 'K': // Kd, Ks, or Ka
                        assert(currentShader);
                        switch (line[1]) {
                            case 'd':   // Kd - diffuse
                                readvec3(line+2, &currentShader->diffuse);
                            break;
                            case 'a':   // Ka - ambient
                                readvec3(line+2, &currentShader->ambient);
                            break;
                            case 's':   // Ks - specular
                                readvec3(line+2, &currentShader->specular);
                            break;
                            default:
                                fprintf(stderr, "Unknown 'K' property: '%s'\n", line);
                            break;
                        }
                    break;

                    case 'N':   // Ns or Ni
                        assert(currentShader);
                        switch(line[1]) {
                            case 'i':   // Ni - index of refreaction
                                currentShader->index = atof(line+2);
                            break;
                            case 's':   // Ns - shininess 0..1000
                                currentShader->exponent = 10.0f* atof(line+2);
                            break;
                            default:
                                fprintf(stderr, "Unknown 'N' property: '%s'\n", line);
                            break;
                        }
                    break;

                    case 'T':   // Tf
#if 0
                        switch(line[1]) {
                            case 'f':   // Tf
                                readvec3(line+2, &currentShader->transmit);
                                sub3(kOne, &currentShader->transmit, &currentShader->transmit);
                                //currentShader->Kt = 1.0;
                            break;
                                default:debug("Unknown 'T' property: '" << line << "'\n");
                            break;
                        }
#endif
                    break;

                    case 'i':   // illum
                        fprintf(stderr, "Ignoring illum (i)\n");
                    break;

                    case 'd':   // dissolve
                        // TODO (SSERAY) currentShader->Kt = 1.0 - atof(line.c_str() + 2);
                        fprintf(stderr, "Ignoring dissolve (d)\n");
                    break;

                    case 'm':   // map_Kd, map_Ks, map_Ka
                        {
#if 0
                            // TODO
                            vector<string> args = split(line);
                            string filename = args[args.size() - 1];
                            string filename = fullpath(path, filename);
                            sptr<TextureFile> texture = new TextureFile;
                            texture->filename = filename;
                            warn("LOADED TEXTURE, Dims = %dx%d\n", texture->width(), texture->height());
                            if (args[0] == "map_Kd") {
                                currentShader->diffuse = texture;
                            } else if (args[0] == "map_Ks") {
                                currentShader->specular = texture;
                            } else if (args[0] == "map_Ka") {
                                currentShader->ambient = texture;
                            } else if (args[0] == "map_bump") {
                                // currentShader->bump = texture;
                            } else {
                                debug("Unknown 'm' command '" << args[0] << "'\n");
                            }
#endif
                        }
                    break;

                    case 'b':   // bump
                        if (strcmp(args[0], "bump") == 0) {
                            fprintf(stderr, "Ignoring bump token\n");
                        } else {
                            fprintf(stderr, "Unknown token '%s'\n", args[0]);
                        }
                    break;

                    case '#':   // comment
                        fprintf(stderr, "%s", line);
                    break;

                    case '\n':
                    case '\t':
                    case '\r': // MSDOS CR
                    case ' ':
                        // ignore whitespace
                    break;

                    default:
                        fprintf(stderr, "FIXME: unknown material line '%s'\n", line);
                    break;
                } // switch
            } else {
                // short line
                fprintf(stderr, "Short line: '%s'\n", line);
            }
        } while (!feof(infile));
    } else {
        fprintf(stderr, "Couldn't open material file '%s'\n", filename);
    }

    fclose(infile);
    free(filename);
    return true;
}

Shader* findShader(const char* name, World* world) {
    // TODO: quicksort array of shaders!
    size_t i;
    for (i = 0; i < world->nShaders; i++) {
        if (strcmp(name, world->shaderNames[i]) == 0) {
            return world->shaders[i];
        }
    }
    return NULL;
}


