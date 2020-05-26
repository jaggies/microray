/*
 * vec2.h
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#ifndef VEC2_H_
#define VEC2_H_

typedef struct Vec2 Vec2;

struct Vec2 {
    float x, y;
};

/* Vec2 operations */
extern void vec2(float x, float y, Vec2* result);
extern void add2(const Vec2* a, const Vec2* b, Vec2* result);
extern void addscaled2(const Vec2* a, float s, const Vec2* b, Vec2* result);
extern void sub2(const Vec2* a, const Vec2* b, Vec2* result);
extern void mult2(const Vec2* a, float t, Vec2* result);
extern float dot2(const Vec2* a, const Vec2* b);
extern float normalize2(Vec2* n);
extern void copy2(const Vec2* a, Vec2* result);
extern void negate2(Vec2* a);
extern void multadd2(const Vec2* a, const Vec2* b, const Vec2* c, Vec2* result);

#endif /* VEC2_H_ */
