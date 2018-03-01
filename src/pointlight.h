/*
 * pointlight.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include "light.h"

typedef struct PointLight {
    LightOps* op;
    Vec3 point;
    Vec3 color;
} PointLight;

extern Light* createPointLight(Vec3* position, Vec3* color);

#endif /* POINTLIGHT_H_ */
