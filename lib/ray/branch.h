/*
 * branch.h
 *
 *  Created on: Mar 6, 2018
 *      Author: grantham@plunk.org
 */

#ifndef BRANCH_H_
#define BRANCH_H_

#include "shape.h"

/* A Shape representing two other shapes with a preferential order
 * of intersection depending on ray direction.

 * This was created to contain the branches of a BVH.
 * Would "SpatialBranch" or "BSO" (binary space ordering) or
 * "OrderedPair" or "SpatialPair" be a better name that describes
 * what it actually does?
 */

/* A container with a shape and its precomputed bounds */
typedef struct _BoundedShape {
    Vec3 boxmin;
    Vec3 boxmax;
    Shape *shape;
} BoundedShape;

/* Two shapes and their bounds with a spatial ordering */
typedef struct _Branch {
    ShapeOps* op; /* these must be first */
    // Shader* shader; // TODO: Branch is not a shape, this could crash
    const Vec3* dir; // pointer to one of the standard axis
    BoundedShape closerChild;
    BoundedShape fartherChild;
} Branch;

extern Shape* createBranch(Shape *closer, Shape *farther, const Vec3 *dir);

#endif /* BRANCH_H_ */
