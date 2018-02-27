#ifndef WORLD_H
#define WORLD_H

#include <shape.h>
#include <light.h>
#include <camera.h>
#include <shader.h>

#define MAXSHAPES 100
#define MAXSHADERS 20
#define MAXLIGHTS 8

typedef struct World {
    Shape* shapes[MAXSHAPES];
    Shader* shaders[MAXSHADERS];
	const char* shaderNames[MAXSHADERS];
    Light* lights[MAXLIGHTS];
    Camera* camera;
    int nShapes;
	int nShaders;
    int nLights;
    Vec3 background; // background colors
    float epsilon; // intersection slop
	int width; // image width
	int height; // image height
} World;

World* createWorld();

#endif // WORLD_H

