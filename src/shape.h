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
typedef struct Shader Shader;

typedef struct ShapeOps {
    int (*intersect)(struct Shape* shape, Ray* ray, float *tmax);
    void (*normal)(struct Shape* shape, struct Hit* hit, Vec3 *n);
    void (*uv)(struct Shape* shape, struct Hit* hit, Vec2 * uv);
    void (*bounds)(struct Shape*, Vec3* min, Vec3* max);
} ShapeOps;

typedef struct Shape {
    ShapeOps *op; // These must be first in all "subclasses"
    Shader* shader; 
} Shape;

#endif /* SHAPE_H_ */
