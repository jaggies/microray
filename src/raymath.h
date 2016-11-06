/*
 * raymath.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef RAYMATH_H_
#define RAYMATH_H_

#include <math.h>
#include "vec2.h"
#include "vec3.h"

extern void reflectionDirection(Vec3* incident, Vec3* normal, Vec3* result);
extern int transmisionDirection(float eta1, float eta2, Vec3* incident, Vec3* normal, Vec3* result);

extern float Radians(float degrees);
extern float Degrees(float radians);

#endif /* RAYMATH_H_ */
