/*
 * shape.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef SHAPEOPS_H_
#define SHAPEOPS_H_

#include "ray.h"

struct Shape;
typedef struct ShapeOps {
    int (*intersect)(struct Shape* shape, Ray* ray, float *tmax);
    void (*normal)(struct Shape* shape, Ray* ray, float t, Vec3 *n);
} ShapeOps;

#endif /* SHAPEOPS_H_ */
