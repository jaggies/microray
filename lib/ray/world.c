#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "world.h"
#include "shape.h"

#define MAX_RAY_DEPTH 4 /* max number of reflected rays */
#define MAX_IMAGE_DEPTH 255
#define RAY_EPSILON 1.0e-5f

World* createWorld() {
    World* world = (World*) malloc(sizeof(World));
    memset(world, 0, sizeof(World));
    world->epsilon = RAY_EPSILON;
    world->raydepth = MAX_RAY_DEPTH;
    world->depth = MAX_IMAGE_DEPTH;
    return world;
}

void destroyWorld(World* world) {
    for (size_t i = 0; i < world->nShapes; i++) {
        Shape* shape = world->shapes[i];
        shape->op->destroy(shape);
    }
    for (size_t i = 0; i < world->nLights; i++) {
        free(world->lights[i]); // TODO: call light->destroy
    }
    for (size_t i = 0; i < world->nShaders; i++) {
        free(world->shaderNames[i]);
        free(world->shaders[i]); // TODO: call shader->destroy
    }
    free(world->shaderNames);
    free(world->shaders);
    free(world->shapes);
    free(world->lights);
    free(world->camera);

    world->shaderNames = NULL;
    world->shaders = NULL;
    world->shapes = NULL;
    world->lights = NULL;
    world->camera = NULL;
    free(world);
}

void addLight(World* world, Light* light) {
    if (light) {
        world->lights = (Light**) realloc(world->lights, (world->nLights+1)*sizeof(Light*));
        if (world->lights) {
            world->lights[world->nLights++] = light;
        } else {
            printf("malloc() failed: too many lights? (%ld)\n", world->nLights);
        }
    }
}

void addShape(World* world, Shape* shape) {
    if (shape) {
        world->shapes = (Shape**) realloc(world->shapes, (world->nShapes+1)*sizeof(Shape*));
        if (world->shapes && shape) {
            world->shapes[world->nShapes++] = shape;
        } else {
            printf("malloc() failed: too many shapes? (%ld)\n", world->nShapes);
        }
    }
}

void addShader(World* world, char* shaderName, Shader* shader) {
    if (shader) {
        world->shaders = (Shader**) realloc(world->shaders, (world->nShaders+1)*sizeof(Shader*));
        world->shaderNames = (char**) realloc(world->shaderNames, (world->nShaders+1)*sizeof(const char*));
        if (world->shaders) {
            world->shaders[world->nShaders] = shader;
            world->shaderNames[world->nShaders] = shaderName; /* allocated in loadPhongShader */
            world->nShaders++;
        } else {
            printf("malloc() failed: too many shaders? (%ld)\n", world->nShaders);
        }
    }
}

