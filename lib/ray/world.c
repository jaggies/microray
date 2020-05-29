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
#include "triangle.h"
#include "world.h"

#define CHUNKSIZE (1<<5)
#define MASK (CHUNKSIZE - 1)

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
        if ((world->nShapes & MASK) == 0) {
            world->shapes = (Shape**) realloc(world->shapes,
                    (world->nShapes+CHUNKSIZE)*sizeof(Shape*));
            assert(world->shapes);
        }
        world->shapes[world->nShapes] = shape;
        return world->nShapes++;
    }
    return -1; // couldn't be added
}

size_t addShader(World* world, const char* shaderName, Shader* shader) {
    if (shader) {
        if ((world->nShaders & MASK) == 0) {
            world->shaders = (Shader**) realloc(world->shaders,
                    (world->nShaders+CHUNKSIZE)*sizeof(Shader*));
            world->shaderNames = (char**) realloc(world->shaderNames,
                    (world->nShaders+CHUNKSIZE)*sizeof(const char*));
        }
        assert(world->shaders);
        assert(world->shaderNames);
        // TODO: insert shader in proper location to avoid sorting later...
        world->shaders[world->nShaders] = shader;
        world->shaderNames[world->nShaders] = strdup(shaderName);
        return world->nShaders++;
    }
    return -1; // couldn't be added
}

size_t addPoint(World* world, Vec3* point) {
    if (point) {
        if ((world->nPoints & MASK) == 0) {
            world->points = (Vec3*) realloc(world->points,
                    (world->nPoints+CHUNKSIZE)*sizeof(Vec3));
            assert(world->points);
        }
        world->points[world->nPoints] = *point;
        return world->nPoints++;
    }
    return -1; // couldn't be added
}

size_t addNormal(World* world, Vec3* normal) {
    if (normal) {
        if ((world->nNormals & MASK) == 0) {
            world->normals = (Vec3*) realloc(world->normals,
                    (world->nNormals+CHUNKSIZE)*sizeof(Vec3));
            assert(world->normals);
        }
        world->normals[world->nNormals] = *normal;
        return world->nNormals++;
    }
    return -1; // couldn't be added
}

size_t addUv(World* world, Vec2* uv) {
    if (uv) {
        if ((world->nUvs & MASK) == 0) {
            world->uvs = (Vec2*) realloc(world->uvs,
                    (world->nUvs+CHUNKSIZE)*sizeof(Vec2));
            assert(world->uvs);
        }
        world->uvs[world->nUvs] = *uv;
        return world->nUvs++;
    }
    return -1; // couldn't be added
}

size_t addFace(World* world, Face* face) {
    if (face) {
        if ((world->nFaces & MASK) == 0) {
            world->faces = (Face**) realloc(world->faces,
                    (world->nFaces+CHUNKSIZE)*sizeof(Face*));
            assert(world->faces);
        }
        world->faces[world->nFaces] = face;
        return world->nFaces++;
    }
    return -1; // couldn't be added
}

void generateTriangles(World* world, Face* face, Shader* shader) {
    static const Vec2 uv = {0.0f, 0.0f}; // default coordinate if none specified
    const Vec3* p0 = &world->points[face->vertexIndex[0]];
    const Vec2* uv0 = face->nTextureIndex > 0
            && face->textureIndex[0] >= 0 ? &world->uvs[face->textureIndex[0]] : &uv;
    size_t i;
    for (i = 1; i < face->nVertexIndex - 1; i++) {
        const Vec3* p1 = &world->points[face->vertexIndex[i]];
        const Vec3* p2 = &world->points[face->vertexIndex[i+1]];
        const Vec2* uv1 = face->nTextureIndex > 0
                && face->textureIndex[i] >= 0 ? &world->uvs[face->textureIndex[i]] : &uv;
        const Vec2* uv2 = face->nTextureIndex > 0
                && face->textureIndex[i+1] >= 0 ? &world->uvs[face->textureIndex[i+1]] : &uv;
        addShape(world, createTriangle(p0, p1, p2, uv0, uv1, uv2, shader));
    }
}

