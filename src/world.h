#ifndef WORLD_H
#define WORLD_H

#include <shape.h>
#include <light.h>
#include <camera.h>

#define MAXSHAPES 10
#define MAXLIGHTS 2

typedef struct World {
    Shape* shapes[MAXSHAPES];
    Light* lights[MAXLIGHTS];
    Camera* camera;
    int nShapes;
    int nLights;
    Vec3 background; // background colors
    float epsilon; // intersection slop
	int width; // image width
	int height; // image height
} World;

World* createWorld();

#endif // WORLD_H

