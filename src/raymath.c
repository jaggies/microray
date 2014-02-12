/*
 * raymath.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include "raymath.h"

void add(Vec3* a, Vec3* b, Vec3* result) {
    result->x = a->x + b->x;
    result->y = a->y + b->y;
    result->z = a->z + b->z;
}

void addscaled(Vec3* a, float s, Vec3* b, Vec3* result) {
    result->x = a->x + s * b->x;
    result->y = a->y + s * b->y;
    result->z = a->z + s * b->z;
}

void sub(Vec3* a, Vec3* b, Vec3* result) {
    result->x = a->x - b->x;
    result->y = a->y - b->y;
    result->z = a->z - b->z;
}

void mult(Vec3* a, float t, Vec3* result) {
    result->x = a->x * t;
    result->y = a->y * t;
    result->z = a->z * t;
}

void cross(Vec3* a, Vec3* b, Vec3* result) {
    Vec3 tmp;
    tmp.x = a->y * b->z - a->z * b->y;
    tmp.y = a->z * b->x - a->x * b->z;
    tmp.z = a->x * b->y - a->y * b->x;
    copy(&tmp, result);
}

float dot(Vec3* a, Vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void normalize(Vec3*n) {
    float d2 = dot(n, n);
    mult(n, 1.0f / sqrt(d2), n);
}

void copy(Vec3* a, Vec3* result) {
    result->x = a->x;
    result->y = a->y;
    result->z = a->z;
}

void negate(Vec3* a) {
    a->x = -a->x;
    a->y = -a->y;
    a->z = -a->z;
}

void makeVec3(float x, float y, float z, Vec3* result) {
    result->x = x;
    result->y = y;
    result->z = z;
}

void reflectionDirection(Vec3* incident, Vec3* normal, Vec3* result)
{
    mult(normal, 2.0f * dot(incident, normal), result);
    sub(incident, result, result);
}

int transmisionDirection(float eta1, float eta2, Vec3* incident, Vec3* normal, Vec3* transmission,
        Vec3* result)
{
    // Heckbert's method (T = eta*I + (eta * costheta1 - sqrt(costheta2sq)) * N)
    float eta = eta1/eta2;
    float costheta1 = -dot(incident, normal);
    float costheta2sq = 1.0 - eta*eta * (1.0 - costheta1 * costheta1);
    if (costheta2sq >= 0.0f) {
        mult(incident, eta, result);
        Vec3 tmp;
        mult(normal, (eta * costheta1 - sqrt(costheta2sq)), &tmp);
        add(&tmp, result, result);
        return 1;
    }
    return 0; // total internal reflection
}


