/*
 * vec3.h
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#ifndef VEC3_H_
#define VEC3_H_

typedef struct Vec3 Vec3;

struct Vec3 {
    float x, y, z;
};

/* Vec3 operations */
extern void vec3(float x, float y, float z, Vec3* result);
extern void add3(const Vec3* a, const Vec3* b, Vec3* result);
extern void addscaled3(const Vec3* a, float s, const Vec3* b, Vec3* result);
extern void sub3(const Vec3* a, const Vec3* b, Vec3* result);
extern void mult3(const Vec3* a, float t, Vec3* result);
extern void cross(const Vec3* a, const Vec3* b, Vec3* result);
extern float dot3(const Vec3* a, const Vec3* b);
extern float normalize3(Vec3* n);
extern void copy3(const Vec3* a, Vec3* result);
extern void negate3(Vec3* a);
extern void multadd3(const Vec3* a, const Vec3* b, const Vec3* c, Vec3* result);

#endif /* VEC3_H_ */
