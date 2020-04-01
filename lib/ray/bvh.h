/*
 * bvh.h
 *
 *  Created on: Mar 6, 2018
 *      Author: grantham@plunk.org
 */

#ifndef BVH_H_
#define BVH_H_

#include "shape.h"

extern Shape* createBVH(Shape **shapes, int nShapes);

#endif /* BVH_H_ */
