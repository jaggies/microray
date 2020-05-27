#ifndef WORLD_H
#define WORLD_H

#include <stddef.h>

#include "os.h"
#include "vec2.h"
#include "vec3.h"
#include "shader.h"
#include "shape.h"
#include "light.h"
#include "camera.h"
#include "phongshd.h"
#include "face.h"

typedef struct World World;

struct World {
    // Shape data
    Shape** shapes;
    Shader** shaders;
    char** shaderNames;
    Light** lights;
    Camera* camera; /* just one of these for now */
    size_t nShapes;
    size_t nShaders; /* number of allocated shaders and shaderNames */
    size_t nLights;

    // Common mesh data. Stored globally to keep track of memory.
    Vec3* points;
    Vec3* normals;
    Vec2* uvs;
    Face** faces;
    size_t nPoints;
    size_t nNormals;
    size_t nUvs;
    size_t nFaces;

    // Settings
    Vec3 background; /* background color */
    float epsilon; /* intersection slop */
    int width; /* image width */
    int height; /* image height */
    int depth; /* image depth */
    int raydepth;
};

// Create an empty world
World* createWorld();

// Destroy and free all resources
void destroyWorld(World*);

// Dump profile metrics to given file or stream
void dumpStats(FILE* fp);

// Returns the index position of the item stored or -1 if it fails.
size_t addLight(World* world, Light* light);
size_t addShape(World* world, Shape* shape);
size_t addShader(World* world, const char* shaderName, Shader* shader);
size_t addPoint(World*, Vec3* point);
size_t addNormal(World*, Vec3* normal);
size_t addUv(World*, Vec2* uv);
size_t addFace(World*, Face* face);

#endif /* WORLD_H */

