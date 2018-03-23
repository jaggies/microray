#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "world.h"

#define RAY_EPSILON 1.0e-5f

World* createWorld() {
    World* world = (World*) malloc(sizeof(World));
    memset(world, sizeof(World), 0);
    printf("Created world, size = %u, ptr=%x, nLights=%d\n",
             sizeof(World), (size_t) world, world->nLights);
    world->epsilon = RAY_EPSILON;
    return world;
}

