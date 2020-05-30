/*
 * stats.c
 *
 *  Created on: May 27, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include "hit.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "branch.h"
#include "leaf.h"
#include "world.h"
#include "stats.h"

#ifdef PROFILE
size_t intersections = 0;
#endif /* PROFILE */

void dumpStats(FILE* fp, World* world) {
#ifdef PROFILE
    extern size_t sphereAllocations;
    extern size_t triangleAllocations;
    extern size_t bvh_leaf_count;
    extern size_t bvh_branch_count;
    fprintf(fp, "Total vertices: %lu (%lu bytes)\n", world->nPoints, world->nPoints*sizeof(Vec3));
    fprintf(fp, "Total normals: %lu (%lu bytes)\n", world->nNormals, world->nNormals*sizeof(Vec3));
    fprintf(fp, "Total texture coords: %lu (%lu bytes)\n", world->nUvs, world->nUvs*sizeof(Vec2));
    fprintf(fp, "Total shapes: %lu\n", world->nShapes);
    fprintf(fp, "Total faces: %lu\n", world->nFaces);
    fprintf(fp, "Total shaders: %lu\n", world->nShaders);
    fprintf(fp, "Total sphere allocations: %lu (%lu bytes)\n",
            sphereAllocations, sphereAllocations*sizeof(Sphere));
    fprintf(fp, "Total triangle allocations: %lu (%lu bytes)\n",
            triangleAllocations, triangleAllocations*sizeof(Triangle));
    fprintf(fp, "BVH Branch allocations: %lu (%lu bytes)\n",
            bvh_branch_count, bvh_branch_count*sizeof(Branch));
    fprintf(fp, "BVH Leaf allocations: %lu (%lu bytes)\n",
            bvh_leaf_count, bvh_leaf_count*sizeof(Leaf));
    fprintf(fp, "Render stats:\n");
    fprintf(fp, "\t%ld intersections\n", intersections);
#endif
}

