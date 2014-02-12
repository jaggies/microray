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


struct Hit;
struct Shape;
struct Shader;

typedef struct ShapeOps {
    int (*intersect)(struct Shape* shape, Ray* ray, float *tmax);
    void (*normal)(struct Shape* shape, struct Hit* hit, Vec3 *n);
    struct Shader* shader; // TODO: find some other way to inherit this
} ShapeOps;

typedef struct Shape {
    ShapeOps op;
} Shape;

#endif /* SHAPE_H_ */
