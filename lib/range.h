/*
 * range.h
 *
 *  Created on: Mar 6, 2018
 *      Author: grantham@plunk.org
 */

#ifndef RANGE_H_
#define RANGE_H_

#include "vec2.h"
#include "vec3.h"
#include "ray.h"

#define RANGE_MIN (-1.0e7f)
#define RANGE_MAX (1.0e7f)

/* These functions perform operations on a Vec2 assuming "x" and "y" */
/* are the lesser and greater two values of a 1D range. */

void range_init(Vec2* r);

int range_is_empty(Vec2* r);

void range_intersect_floats(Vec2* r1, float t0, float t1);

void range_intersect_ray_box(Vec2* range, Vec3* boxmin, Vec3* boxmax, Ray* ray);

#endif /* RANGE_H_ */
