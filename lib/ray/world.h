#ifndef WORLD_H
#define WORLD_H

#include "vec3.h"

typedef struct World World;
typedef struct Shape Shape;
typedef struct Shader Shader;
typedef struct Light Light;
typedef struct Camera Camera;

struct World {
    Shape** shapes;
    Shader** shaders;
    char** shaderNames;
    Light** lights;
    Camera* camera;
    size_t nShapes;
    size_t nShaders;
    size_t nLights;
    Vec3 background; /* background colors */
    float epsilon; /* intersection slop */
    int width; /* image width */
    int height; /* image height */
    int depth; /* image depth */
    int raydepth;
};

World* createWorld();
void destroyWorld(World*);

void addLight(World* world, Light* light);
void addShape(World* world, Shape* shape);
void addShader(World* world, char* shaderName, Shader* shader);

#endif /* WORLD_H */

