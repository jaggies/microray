#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"
#include "vec3.h"
#include "hit.h"
#include "phongshd.h"
#include "checkshd.h"
#include "shape.h"
#include "sphere.h"
#include "triangle.h"
#include "bvh.h"
#include "branch.h"
#include "leaf.h"
#include "pointlit.h"
#include "perspcam.h"
#include "world.h"
#include "loader.h"

#include "waveload.h"

#define MAXNAME 16
#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 200

enum { COMMENT = 0, SPHERE, TRIANGLE, PHONG, PERSPECTIVECAMERA,
        POINTLIGHT, CHECKERBOARD, BACKGROUND, RESOLUTION, LOAD };
const char *tokens[] = { "#", "sphere", "triangle", "phongshader", "perspectivecamera",
        "pointlight", "checkerboard", "background", "resolution", "load" };

static const int kTokens = (sizeof(tokens) / sizeof(tokens[0]));
static const char* DELIM = " \t\n";
static Shader* defaultShader = 0;

static bool loadNative(World* world, const char* fromPath);

bool loadWorld(World* world, const char* fromPath) {
    bool result = false;
    char* suffix = strrchr(fromPath, '.');
    if (suffix && 0 == strcasecmp(suffix, ".obj")) {
        result = loadWavefront(world, fromPath, "");
    } else if (suffix && 0 == strcasecmp(suffix, ".scn")) {
        result = loadNative(world, fromPath);
    } else {
        printf("Unknown file type '%s'\n", fromPath);
    }
#ifdef DEBUG
    if (world->nShapes == 0) {
        printf("World contains no shapes\n");
    }
    if (world->nLights == 0) {
        printf("World contains no lights\n");
    }
    if (!world->camera) {
        printf("World contains no camera\n");
    }
#endif
    return result;
}

static Shader* getShaderOrDefault(World* world, const char* shaderName) {
    Shader* shader = getShader(world, shaderName);
    if (!shader) {
        if (!defaultShader) {
            Vec3 diffuse, specular, ambient;
            vec3(1.0f,1.0f,1.0f,&diffuse);
            vec3(1.0f,1.0f,1.0f,&specular);
            vec3(0.0f,0.0f,0.0f,&ambient);
            defaultShader = (Shader*) createPhongShader(&diffuse, &specular, &ambient, 20.0f, 1.0f, 0.0f, 0.0f);
        }
        return defaultShader;
    }
    return shader;
}

static Vec3 loadBackground(char* args) {
    Vec3 color;
	color.x = atof(strtok(args, DELIM));
	color.y = atof(strtok(0, DELIM));
	color.z = atof(strtok(0, DELIM));
    return color;
}

void loadResolution(World* world, char* args) {
    world->width = atoi(strtok(args, DELIM));
    world->height = atoi(strtok(0, DELIM));
}

void loadFile(World* world, char* args) {
    char* fname = strtok(args, DELIM);
    loadWorld(world, fname);
}

static PhongShader* loadPhongShader(char* args, char** outname) {
    Vec3 diffuse, specular, ambient;
    float exponent, index, reflect, transmit;
    *outname = strtok(args, DELIM);
	diffuse.x = atof(strtok(0, DELIM));
	diffuse.y = atof(strtok(0, DELIM));
	diffuse.z = atof(strtok(0, DELIM));
	specular.x = atof(strtok(0, DELIM));
	specular.y = atof(strtok(0, DELIM));
	specular.z = atof(strtok(0, DELIM));
	ambient.x = atof(strtok(0, DELIM));
	ambient.y = atof(strtok(0, DELIM));
	ambient.z = atof(strtok(0, DELIM));
	exponent = atof(strtok(0, DELIM));
	index = atof(strtok(0, DELIM));
	reflect = atof(strtok(0, DELIM));
	transmit = atof(strtok(0, DELIM));
    return createPhongShader(&diffuse, &specular, &ambient, exponent, index, reflect, transmit);
}

static Shader* loadCheckerboardShader(World* world, char* args, char** outname) {
    char* oddName;
    char* evenName;
    Vec2 scale, bias;
    *outname = strtok(args, DELIM);
	oddName = strtok(0, DELIM);
	evenName = strtok(0, DELIM);
    scale.x = atof(strtok(0, DELIM));
	scale.y = atof(strtok(0, DELIM));
	bias.x = atof(strtok(0, DELIM));
	bias.y = atof(strtok(0, DELIM));
    return createCheckerboardShader(getShader(world, oddName), getShader(world, evenName), &scale, &bias);
}

static Shape* loadSphere(World* world, char* args) {
    char* material = strtok(args, DELIM);
    float x = atof(strtok(0, DELIM));
    float y = atof(strtok(0, DELIM));
    float z = atof(strtok(0, DELIM));
    float r = atof(strtok(0, DELIM));
    return createSphere(x, y, z, r, getShaderOrDefault(world, material));
}

static void loadTriangle(World* world, char* args) {
    Vec3 p1, p2, p3;
    Vec2 uv1, uv2, uv3;
    char* material = strtok(args, DELIM);
    Face* face = createFace();
    vec3(atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), &p1);
    vec3(atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), &p2);
    vec3(atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), &p3);
    addFaceVertex(face, addPoint(world, &p1));
    addFaceVertex(face, addPoint(world, &p2));
    addFaceVertex(face, addPoint(world, &p3));
    vec2(atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), &uv1);
    vec2(atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), &uv2);
    vec2(atof(strtok(0, DELIM)), atof(strtok(0, DELIM)), &uv3);
    addFaceTexture(face, addUv(world, &uv1));
    addFaceTexture(face, addUv(world, &uv2));
    addFaceTexture(face, addUv(world, &uv3));
    generateTriangles(world, face, getShaderOrDefault(world, material));
    destroyFace(face);
}

static Camera* loadPerspectiveCamera(World* world, char* args) {
    Vec3 from, at, up;
    float fov, aspect;
    from.x = atof(strtok(args, DELIM)); from.y = atof(strtok(0, DELIM)); from.z = atof(strtok(0, DELIM));
    at.x = atof(strtok(0, DELIM)); at.y = atof(strtok(0, DELIM)); at.z = atof(strtok(0, DELIM));
    up.x = atof(strtok(0, DELIM)); up.y = atof(strtok(0, DELIM)); up.z = atof(strtok(0, DELIM));
    fov = atof(strtok(0, DELIM));
    aspect = atof(strtok(0, DELIM));
    if (world->width != 0 && world->height != 0) {
        aspect = ((float) world->width / world->height) / aspect;
        printf("Computed aspect: %f\n", aspect);
    }
    return createPerspectiveCamera(&from, &at, &up, fov, aspect);
}

static Light* loadPointLight(char* args) {
    Vec3 position, color;
	position.x = atof(strtok(args, DELIM)); position.y = atof(strtok(0, DELIM)); position.z = atof(strtok(0, DELIM));
	color.x = atof(strtok(0, DELIM)); color.y = atof(strtok(0, DELIM)); color.z = atof(strtok(0, DELIM));
    return createPointLight(&position, &color);
}

bool loadNative(World* world, const char* fromPath)
{
    int line = 0;
    char buffer[100];
    FILE *fp;
    fp = fopen(fromPath, "r");
    if (!fp) {
        return 0;
    }
    printf("Reading native file %s\n", fromPath);
    while (!feof(fp)) {
        int i;
        char* ptr;
        char* linestr = fgets(buffer, sizeof(buffer), fp);
        int foundToken = -1; /* unknown */
        if (!linestr) {
            break;
        }
        line++;
        for (i = 0; i < kTokens; i++) {
            const char* token;
            ptr = linestr;
            token = tokens[i];
            while (*token == *ptr && *token != 0 && *ptr != 0) {
                ptr++;
                token++;
            }
            if (*token == 0) { /* found */
                foundToken = i;
                break; /* token found */
            }
        }
        if (foundToken == -1) {
            printf("Bad token, line %d: \n\t%s", line, ptr);
            continue; /* ignore rest of line */
        } else {
            char* name;
            Shader* shader;
            switch(foundToken) {
                case COMMENT:
                    break; /* ignore */
                case PERSPECTIVECAMERA:
                    world->camera = loadPerspectiveCamera(world, ptr);
                    break;
                case SPHERE:
                    addShape(world, loadSphere(world, ptr));
                    break;
                case POINTLIGHT:
                    addLight(world, loadPointLight(ptr));
                    break;
                case PHONG:
                    shader = (Shader*) loadPhongShader(ptr, &name);
                    addShader(world, name, shader);
                    break;
                case BACKGROUND:
                    world->background = loadBackground(ptr);
                    break;
                case TRIANGLE:
                    loadTriangle(world, ptr);
                    break;
                case CHECKERBOARD:
                    shader = loadCheckerboardShader(world, ptr, &name);
                    addShader(world, name, shader);
                    break;
                case RESOLUTION:
                    loadResolution(world, ptr);
                    break;
                case LOAD:
                    loadFile(world, ptr);
                    break;
                    printf("Unimplemented token %s, args:%s", tokens[foundToken], ptr);
            }
        }
    }
    fclose(fp);

    fclose(fp);

    if(!getenv("NO_BVH")) {
        Shape *root;
        printf("Creating BVH...");
        fflush(stdout);
        if(1) {
            root = createBVH(world->shapes, world->nShapes);
        } else {
            root = createLeaf(world->shapes, world->nShapes);
        }
        printf("done!\n");

        // TODO: This is a messy and non-obvious way to clear the list of objects from world.
        free(world->shapes); // shapes is now replicated in above BVH or Leaf node.
        world->nShapes = 0;
        world->shapes = NULL; // ensure this is reallocated.
        addShape(world, root);
    }

    // Fix aspect
    if (world->height == 0 || world->width == 0) {
        world->width = DEFAULT_WIDTH;
        world->height = DEFAULT_HEIGHT;
    }

    return true;
}

