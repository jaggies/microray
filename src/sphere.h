/*
 * sphere.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "shape.h"

typedef struct Sphere {
    ShapeOps op;
    Vec3 position;
    float radius2;
} Sphere;

extern Shape* createSphere(float x, float y, float z, float radius);

#endif /* SPHERE_H_ */
