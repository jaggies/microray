/*
 * shape.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include <stdlib.h>
#include "ray.h"

struct Shape;
struct Hit;

typedef struct ShapeOps {
    int (*intersect)(struct Shape* shape, Ray* ray, float *tmax);
    void (*normal)(struct Shape* shape, Ray* ray, struct Hit* hit, Vec3 *n);
} ShapeOps;

typedef struct Shape {
    ShapeOps op;
} Shape;

#endif /* SHAPE_H_ */
