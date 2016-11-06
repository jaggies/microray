/*
 * hit.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef HIT_H_
#define HIT_H_

#include "vec2.h"
#include "vec3.h"
#include "shape.h"

typedef struct Hit {
    float t;
    int  inShadow;  // whether this hit was in shadow (used by shade())
    Vec3 point;     // point of intersection
    Vec3 normal;    // normal at intersection
    Vec2 uv;        // uv at intersection
    float index;    // index of refraction of current medium
    Ray  lightRay;  // lightRay from intersection to current light
    Vec3 reflect;   // reflected ray direction
    Shape* best;    // tracks closest object
} Hit;

extern void clearHit(Hit* hit);

#endif /* HIT_H_ */
