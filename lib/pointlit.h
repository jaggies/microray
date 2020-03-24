/*
 * pointlight.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include "light.h"

typedef struct PointLight PointLight;

struct PointLight {
    LightOps* op;
    Vec3 color;
    Vec3 point;
};

extern Light* createPointLight(Vec3* position, Vec3* color);

#endif /* POINTLIGHT_H_ */
