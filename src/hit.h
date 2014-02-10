/*
 * hit.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef HIT_H_
#define HIT_H_

#include "vec3.h"
#include "shape.h"

typedef struct Hit {
    float t;
    Vec3 point;
    Vec3 normal;
    Vec3 uv;
    Shape* best;
} Hit;

extern void clearHit(Hit* hit);

#endif /* HIT_H_ */
