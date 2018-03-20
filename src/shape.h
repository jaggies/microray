/*
 * shape.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include "ray.h"
#include "hit.h"

typedef struct _Shape Shape;
typedef struct _Shader Shader;

typedef struct _ShapeOps {
    int (*intersect)(Shape* shape, Ray* ray, Hit* hit);
    void (*normal)(Shape* shape, Hit* hit, Vec3 *n);
    void (*uv)(Shape* shape, Hit* hit, Vec2 * uv);
    void (*bounds)(Shape*, Vec3* min, Vec3* max);
} ShapeOps;

typedef struct _Shape {
    ShapeOps *op; /* These must be first in all "subclasses" */
    Shader* shader; 
} Shape;

#endif /* SHAPE_H_ */
