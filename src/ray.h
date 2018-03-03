/*
 * ray.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef RAY_H_
#define RAY_H_

#include "raymath.h"

typedef struct Ray {
    Vec3 point;
    Vec3 dir;
} Ray;

#endif /* RAY_H_ */
