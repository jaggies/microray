/*
 * lightops.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef LIGHTOPS_H_
#define LIGHTOPS_H_

#include "vec3.h"
#include "ray.h"

struct Light;
typedef struct LightOps {
    void (*makeRay)(struct Light* light, Vec3* point, Ray* ray);
} LightOps;

#endif /* LIGHTOPS_H_ */
