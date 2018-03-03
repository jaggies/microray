#include <stdlib.h>
#include <string.h>
#include "world.h"

#define RAY_EPSILON 1.0e-5f

World* createWorld() {
    World* world = (World*) malloc(sizeof(World));
    memset(world, sizeof(World), 0);
    world->epsilon = RAY_EPSILON;
    return world;
}

