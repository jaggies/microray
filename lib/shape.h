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

typedef struct Shader Shader;
typedef struct Shape Shape;
typedef struct ShapeOps ShapeOps;

struct Shape {
    ShapeOps *op; /* These must be first in all "subclasses" */
    Shader* shader; 
};

struct ShapeOps {
    int (*intersect)(Shape* shape, Ray* ray, Hit* hit);
    void (*normal)(Shape* shape, Hit* hit, Vec3 *n);
    void (*uv)(Shape* shape, Hit* hit, Vec2 * uv);
    void (*bounds)(Shape*, Vec3* min, Vec3* max);
};

#endif /* SHAPE_H_ */
