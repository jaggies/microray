/*
 * branch.h
 *
 *  Created on: Mar 6, 2018
 *      Author: grantham@plunk.org
 */

#ifndef BRANCH_H_
#define BRANCH_H_

#include "shape.h"

// A Shape representing two other shapes with a preferential order
// of intersection depending on ray direction.

// This was created to contain the branches of a BVH.
// Would "SpatialBranch" or "BSO" (binary space ordering) or
// "OrderedPair" or "SpatialPair" be a better name that describes
// what it actually does?

// A container with a shape and its precomputed bounds
typedef struct BoundedShape {
    Vec3 boxmin;
    Vec3 boxmax;
    Shape *shape;
} BoundedShape;

// Two shapes and their bounds with a spatial ordering
typedef struct Branch {
    ShapeOps* op; // these must be first
    Vec3 dir;
    BoundedShape closerChild;
    BoundedShape fartherChild;
} Branch;

extern Shape* createBranch(Shape *closer, Shape *farther);

#endif /* BRANCH_H_ */
