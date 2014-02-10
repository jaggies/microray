/*
 * raymath.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef RAYMATH_H_
#define RAYMATH_H_

#include <math.h>
#include "vec3.h"

extern void add(Vec3* a, Vec3* b, Vec3* result);
extern void sub(Vec3* a, Vec3* b, Vec3* result);
extern void mult(Vec3* a, float t, Vec3* result);
extern void cross(Vec3* a, Vec3* b, Vec3* result);
extern float dot(Vec3* a, Vec3* b);
extern void normalize(Vec3*n);
extern void copy(Vec3* a, Vec3* result);
extern void makeVec3(float x, float y, float z, Vec3* result);

#endif /* RAYMATH_H_ */
