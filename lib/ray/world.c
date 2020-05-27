#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hit.h"
#include "light.h"
#include "shader.h"
#include "camera.h"
#include "shape.h"
#include "sphere.h"
#include "world.h"

#define MAX_RAY_DEPTH 4 /* max number of reflected rays */
#define MAX_IMAGE_DEPTH 255
#define RAY_EPSILON 1.0e-5f

World* createWorld() {
    World* world = (World*) calloc(1, sizeof(World));
    world->epsilon = RAY_EPSILON;
    world->raydepth = MAX_RAY_DEPTH;
    world->depth = MAX_IMAGE_DEPTH;
    return world;
}

void destroyWorld(World* world) {
    size_t i;
    for (i = 0; i < world->nShapes; i++) {
        Shape* shape = world->shapes[i];
        shape->op->destroy(shape);
    }
    for (i = 0; i < world->nLights; i++) {
        free(world->lights[i]); // TODO: call light->destroy
    }
    for (i = 0; i < world->nShaders; i++) {
        free(world->shaderNames[i]);
        free(world->shaders[i]); // TODO: call shader->destroy
    }
    for (i = 0; i < world->nFaces; i++) {
        destroyFace(world->faces[i]);
    }
    free(world->shapes);
    free(world->shaders);
    free(world->shaderNames);
    free(world->lights);
    free(world->camera);
    free(world->points);
    free(world->normals);
    free(world->uvs);
    free(world->faces);

    world->shaderNames = NULL;
    world->shaders = NULL;
    world->shapes = NULL;
    world->lights = NULL;
    world->camera = NULL;
    free(world);
}

size_t addLight(World* world, Light* light) {
    if (light) {
        world->lights = (Light**) realloc(world->lights, (world->nLights+1)*sizeof(Light*));
        assert(world->lights);
        world->lights[world->nLights] = light;
        return world->nLights++;
    }
    return -1; // couldn't be added
}

size_t addShape(World* world, Shape* shape) {
    if (shape) {
        world->shapes = (Shape**) realloc(world->shapes, (world->nShapes+1)*sizeof(Shape*));
        assert(world->shapes);
        world->shapes[world->nShapes] = shape;
        return world->nShapes++;
    }
    return -1; // couldn't be added
}

size_t addShader(World* world, const char* shaderName, Shader* shader) {
    if (shader) {
        // TODO: insert shader in proper location to avoid sorting later...
        world->shaders = (Shader**) realloc(world->shaders, (world->nShaders+1)*sizeof(Shader*));
        world->shaderNames = (char**) realloc(world->shaderNames, (world->nShaders+1)*sizeof(const char*));
        assert(world->shaders);
        assert(world->shaderNames);
        world->shaders[world->nShaders] = shader;
        world->shaderNames[world->nShaders] = strdup(shaderName);
        return world->nShaders++;
    }
    return -1; // couldn't be added
}

size_t addPoint(World* world, Vec3* point) {
    if (point) {
        world->points = (Vec3*) realloc(world->points, (world->nPoints+1)*sizeof(Vec3));
        assert(world->points);
        world->points[world->nPoints] = *point;
        return world->nPoints++;
    }
    return -1; // couldn't be added
}

size_t addNormal(World* world, Vec3* normal) {
    if (normal) {
        world->normals = (Vec3*) realloc(world->normals, (world->nNormals+1)*sizeof(Vec3));
        assert(world->normals);
        world->normals[world->nNormals] = *normal;
        return world->nNormals++;
    }
    return -1; // couldn't be added
}

size_t addUv(World* world, Vec2* uv) {
    if (uv) {
        world->uvs = (Vec2*) realloc(world->uvs, (world->nUvs+1)*sizeof(Vec2));
        assert(world->uvs);
        world->uvs[world->nUvs] = *uv;
        return world->nUvs++;
    }
    return -1; // couldn't be added
}

size_t addFace(World* world, Face* face) {
    if (face) {
        world->faces = (Face**) realloc(world->faces, (world->nFaces+1)*sizeof(Face*));
        assert(world->faces);
        world->faces[world->nFaces] = face;
        return world->nFaces++;
    }
    return -1; // couldn't be added
}
