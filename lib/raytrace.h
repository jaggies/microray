/*
 * raytrace.h
 *
 *  Created on: Feb 26, 2018
 *      Author: jmiller
 */

#ifndef RAYTRACE_H_
#define RAYTRACE_H_

extern Vec3* shade(Ray* ray, World* world, Hit* hit, Vec3* color, int maxdepth);
extern void trace(Ray* ray, const Shape* ignore, World* world, Vec3* color, int maxdepth);
extern int shadow(Ray* ray, const Shape* ignore, World* world);

typedef void (*PixelCB)(uint16_t x, uint16_t y, uint8_t rgb[3], void* userdata);
extern void renderImage(World* world, PixelCB pixel, void* data);

#endif /* RAYTRACE_H_ */
