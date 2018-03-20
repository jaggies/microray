/*
 * sphere.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include "shape.h"

typedef struct _Sphere {
    ShapeOps* op; /* these must be first */
    Shader* shader;
    Vec3 position;
    float radius; /* used for bounds calculation */
    float radius2;
} Sphere;

extern Shape* createSphere(float x, float y, float z, float radius, Shader* shader);

#endif /* SPHERE_H_ */
