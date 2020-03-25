#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "world.h"

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

