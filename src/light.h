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

typedef struct Light Light;
typedef struct LightOps LightOps;

struct LightOps {
    void (*makeRay)(Light* light, Vec3* point, Ray* ray);
};

struct Light {
    LightOps* op; /* These have to be the first two in all subclasses */
	Vec3 color;
};

#endif /* LIGHT_H_ */
