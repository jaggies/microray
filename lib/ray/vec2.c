/*
 * vec2.c
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#include <math.h>
#include "vec2.h"

void vec2(float x, float y, Vec2* result) {
    result->x = x;
    result->y = y;
}

void add2(const Vec2* a, const Vec2* b, Vec2* result) {
    result->x = a->x + b->x;
    result->y = a->y + b->y;
}

void addscaled2(const Vec2* a, float s, const Vec2* b, Vec2* result) {
    result->x = a->x + s * b->x;
    result->y = a->y + s * b->y;
}

void sub2(const Vec2* a, const Vec2* b, Vec2* result) {
    result->x = a->x - b->x;
    result->y = a->y - b->y;
}

void mult2(const Vec2* a, float t, Vec2* result) {
    result->x = a->x * t;
    result->y = a->y * t;
}

float dot2(const Vec2* a, const Vec2* b) {
    return a->x * b->x + a->y * b->y;
}

float normalize2(Vec2*n) {
    float result = sqrt(dot2(n, n));
    mult2(n, 1.0f / result, n);
    return result;
}

void copy2(const Vec2* a, Vec2* result) {
    result->x = a->x;
    result->y = a->y;
}

void negate2(Vec2* a) {
    a->x = -a->x;
    a->y = -a->y;
}

void multadd2(const Vec2* a, const Vec2* b, const Vec2* c, Vec2* result) {
    result->x = a->x * b->x + c->x;
    result->y = a->y * b->y + c->y;
}

