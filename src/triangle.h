/*
 * triangle.h
 *
 *  Created on: Feb 11, 2014
 *      Author: jmiller
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "shape.h"

typedef struct Triangle {
    ShapeOps op;
    Vec3  point0;
    Vec3  edge1;
    Vec3  edge2;
    Vec3  normal0;
} Triangle;

extern Shape* createTriangle(Vec3* p1, Vec3* p2, Vec3* p3, struct Shader* shader);

#endif /* TRIANGLE_H_ */
