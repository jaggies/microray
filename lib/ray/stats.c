/*
 * stats.c
 *
 *  Created on: May 27, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include "stats.h"
#include "hit.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "branch.h"
#include "leaf.h"

#ifdef PROFILE
size_t intersections = 0;
#endif /* PROFILE */

void dumpStats(FILE* fp) {
#ifdef PROFILE
    extern size_t sphereAllocations;
    extern size_t triangleAllocations;
    extern size_t bvh_leaf_count;
    extern size_t bvh_branch_count;
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

