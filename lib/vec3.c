/*
 * vec3.c
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#include <math.h>
#include "vec3.h"

void vec3(float x, float y, float z, Vec3* result) {
    result->x = x;
    result->y = y;
    result->z = z;
}

void add3(Vec3* a, Vec3* b, Vec3* result) {
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

void addscaled3(Vec3* a, float s, Vec3* b, Vec3* result) {
    result->x = a->x + s * b->x;
    result->y = a->y + s * b->y;
    result->z = a->z + s * b->z;
}

void sub3(Vec3* a, Vec3* b, Vec3* result) {
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}

void mult3(Vec3* a, float t, Vec3* result) {
    result->x = a->x * t;
    result->y = a->y * t;
    result->z = a->z * t;
}

void cross(Vec3* a, Vec3* b, Vec3* result) {
    Vec3 tmp;
    tmp.x = a->y * b->z - a->z * b->y;
    tmp.y = a->z * b->x - a->x * b->z;
    tmp.z = a->x * b->y - a->y * b->x;
    copy3(&tmp, result);
}

float dot3(Vec3* a, Vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void normalize3(Vec3*n) {
    float d2 = dot3(n, n);
    mult3(n, 1.0f / sqrt(d2), n);
}

void copy3(Vec3* a, Vec3* result) {
    result->x = a->x;
    result->y = a->y;
    result->z = a->z;
}

void negate3(Vec3* a) {
    a->x = -a->x;
    a->y = -a->y;
    a->z = -a->z;
}

void multadd3(Vec3* a, Vec3* b, Vec3* c, Vec3* result) {
    result->x = a->x * b->x + c->x;
    result->y = a->y * b->y + c->y;
    result->z = a->z * b->z + c->z;
}

