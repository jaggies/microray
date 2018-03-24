#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "world.h"

#define RAY_EPSILON 1.0e-5f

World* createWorld() {
    World* world = (World*) malloc(sizeof(World));
    memset(world, 0, sizeof(World));
    world->epsilon = RAY_EPSILON;
    return world;
}

