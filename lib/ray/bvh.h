/*
 * bvh.h
 *
 *  Created on: Mar 6, 2018
 *      Author: grantham@plunk.org
 */

#ifndef BVH_H_
#define BVH_H_

#include "shape.h"

/* Container for the root of a BVH tree */
struct BVH {
    ShapeOps* op; /* these must be first */
    Shader* shader; /* not currently used */
    Shape* root; /* pointer to root of tree */
    Shape** shapes; /* for housekeeping (cleanup) */
    size_t nShapes;
};

typedef struct BVH BVH;

extern Shape* createBVH(Shape **shapes, int nShapes);

#endif /* BVH_H_ */
