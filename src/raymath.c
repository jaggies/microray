/*
 * raymath.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <math.h>
#include "raymath.h"

void reflectionDirection(Vec3* incident, Vec3* normal, Vec3* result)
{
    mult3(normal, 2.0f * dot3(incident, normal), result);
    sub3(incident, result, result);
}

int transmisionDirection(float eta1, float eta2, Vec3* incident, Vec3* normal, Vec3* result)
{
    // Heckbert's method (T = eta*I + (eta * costheta1 - sqrt(costheta2sq)) * N)
    float eta = eta1/eta2;
    float costheta1 = -dot3(incident, normal);
    float costheta2sq = 1.0 - eta*eta * (1.0 - costheta1 * costheta1);
    if (costheta2sq >= 0.0f) {
        Vec3 tmp;
        mult3(incident, eta, result);
        mult3(normal, (eta * costheta1 - sqrt(costheta2sq)), &tmp);
        add3(&tmp, result, result);
        return 1;
    }
    return 0; // total internal reflection
}

float Radians(float degrees) {
    return M_PI * degrees / 180.0f;
}

float Degrees(float radians) {
    return 180.0f * radians / M_PI;
}
