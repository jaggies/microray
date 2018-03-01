/*
 * light.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "vec3.h"
#include "ray.h"

struct Light;
typedef struct LightOps {
    void (*makeRay)(struct Light* light, Vec3* point, Ray* ray);
} LightOps;

typedef struct Light {
    LightOps* op;
} Light;

#endif /* LIGHT_H_ */
