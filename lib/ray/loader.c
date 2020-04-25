#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vec3.h"
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

#define MAXNAME 16
#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 200

enum { COMMENT = 0, SPHERE, TRIANGLE, PHONG, PERSPECTIVECAMERA, POINTLIGHT, CHECKERBOARD, BACKGROUND, RESOLUTION };
const char *tokens[] = { "#", "sphere", "triangle", "phongshader", "perspectivecamera", "pointlight", "checkerboard", "background", "resolution" };
const int kTokens = (sizeof(tokens) / sizeof(tokens[0]));
static const char* DELIM = " \t";
static Shader* defaultShader = 0;
#ifndef strdup
extern char* strdup(const char* str);
#endif

static void addLight(World* world, Light* light) {
    if (light) {
        if (world->nLights < MAXLIGHTS) {
            world->lights[world->nLights++] = light;
        } else {
            printf("Too many lights (%d)!\n", world->nLights);
        }
    }
}

static void addShape(World* world, Shape* shape) {
    if (shape) {
        if (world->nShapes < MAXSHAPES) {
            world->shapes[world->nShapes++] = shape;
        } else {
            printf("Too many shapes (%d)!\n", world->nShapes);
        }
    }
}

static Shader* getShader(World* world, const char* shaderName) {
    int i;
    for (i = 0; i < world->nShaders; i++) {
        if (strcmp(shaderName, world->shaderNames[i]) == 0) {
            return world->shaders[i];
        }
    }
    printf("Shader %s not found\n", shaderName);
    if (!defaultShader) {
        Vec3 diffuse, specular, ambient;
        vec3(1.0f,1.0f,1.0f,&diffuse);
        vec3(1.0f,1.0f,1.0f,&specular);
        vec3(0.0f,0.0f,0.0f,&ambient);
        defaultShader = createPhongShader(&diffuse, &specular, &ambient, 20.0f, 1.0f, 0.0f, 0.0f);
    }
    return defaultShader;
}

static void addShader(World* world, const char* shaderName, Shader* shader) {
    if (shader) {
        if (world->nShaders < MAXSHADERS) {
            world->shaders[world->nShaders] = shader;
            world->shaderNames[world->nShaders] = shaderName; /* allocated in loadPhongShader */
            world->nShaders++;
        } else {
            printf("Too many shaders(%d)!\n", world->nShaders);
        }
    }
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

static Shader* loadPhongShader(char* args, char** outname) {
    Vec3 diffuse, specular, ambient;
    float exponent, index, reflect, transmit;
    *outname = (char*) strdup(strtok(args, DELIM));
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
    char* name;
    char* oddName;
    char* evenName;
    Vec2 scale, bias;
	name = strtok(args, DELIM);
	oddName = strtok(0, DELIM);
	evenName = strtok(0, DELIM);
    scale.x = atof(strtok(0, DELIM));
	scale.y = atof(strtok(0, DELIM));
	bias.x = atof(strtok(0, DELIM));
	bias.y = atof(strtok(0, DELIM));
    *outname = (char*) strdup(name);
    return createCheckerboardShader(getShader(world, oddName), getShader(world, evenName), &scale, &bias);
}

static Shape* loadSphere(World* world, char* args) {
    char* material = strtok(args, DELIM);
	float x = atof(strtok(0, DELIM));
	float y = atof(strtok(0, DELIM));
	float z = atof(strtok(0, DELIM));
	float r = atof(strtok(0, DELIM));
    return createSphere(x, y, z, r, getShader(world, material));
}

static Shape* loadTriangle(World* world, char* args) {
    Vec3 p1, p2, p3;
    Vec2 uv1, uv2, uv3;
    char* material = strtok(args, DELIM);
	p1.x = atof(strtok(0, DELIM)); p1.y = atof(strtok(0, DELIM)); p1.z = atof(strtok(0, DELIM));
	p2.x = atof(strtok(0, DELIM)); p2.y = atof(strtok(0, DELIM)); p2.z = atof(strtok(0, DELIM));
	p3.x = atof(strtok(0, DELIM)); p3.y = atof(strtok(0, DELIM)); p3.z = atof(strtok(0, DELIM));
	uv1.x = atof(strtok(0, DELIM)); uv1.y = atof(strtok(0, DELIM));
	uv2.x = atof(strtok(0, DELIM)); uv2.y = atof(strtok(0, DELIM));
	uv3.x = atof(strtok(0, DELIM)); uv3.y = atof(strtok(0, DELIM));
    return createTriangle(&p1, &p2, &p3, &uv1, &uv2, &uv3, getShader(world, material));
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

World* loadFile(const char* fromPath)
{
    int line = 0;
    char buffer[100];
    World* world = createWorld();
    FILE *fp;
    printf("Opening file %s\n", fromPath);
    fp = fopen(fromPath, "r");
    if (!fp) {
        return 0;
    }
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
                    shader = loadPhongShader(ptr, &name);
                    addShader(world, name, shader);
                    break;
                case BACKGROUND:
                    world->background = loadBackground(ptr);
                    break;
                case TRIANGLE:
                    addShape(world, loadTriangle(world, ptr));
                    break;
                case CHECKERBOARD:
                    shader = loadCheckerboardShader(world, ptr, &name);
                    addShader(world, name, shader);
                    break;
                case RESOLUTION:
                    loadResolution(world, ptr);
                    break;
                default:
                    printf("Unimplemented token %s, args:%s", tokens[foundToken], ptr);
            }
        }
    }
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
        world->shapes[0] = root;
        world->nShapes = 1;
    }

    // Fix up aspect
    if (world->height == 0 || world->width == 0) {
        world->width = DEFAULT_WIDTH;
        world->height = DEFAULT_HEIGHT;
    }

    return world;
}
