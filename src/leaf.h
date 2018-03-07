/*
 * leaf.h
 *
 *  Created on: Mar 5, 2018
 *      Author: grantham@plunk.org
 */

#ifndef LEAF_H_
#define LEAF_H_

#include "shape.h"

// A Shape representing an array of shapes.

// This was created to contain the leafs of a BVH, but isn't
// specific to BVH.  Would "Group" or "List" be a better name?

typedef struct Leaf {
    ShapeOps* op; // these must be first
    int nShapes;
    Shape **shapes;
} Leaf;

extern Shape* createLeaf(Shape **shapes, int nShapes);

#endif /* LEAF_H_ */
