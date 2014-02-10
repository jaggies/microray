/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include "sphere.h"
#include "hit.h"

#define XRES 20
#define YRES 10

void createRay(float u, float v, Ray* ray) {
    makeVec3(u, v, 1, &ray->point);
    makeVec3(0, 0, -1, &ray->dir);
    normalize(&ray->dir);
}

int main(int argc, char **argv)
{
    const int MAXSHAPES = 10;
    Shape* shapes[MAXSHAPES];
    int n = 0;
    shapes[n++] = createSphere(0, 0, 0, 0.5);
    float du = 1.0f / XRES, dv = 1.0f / YRES;
    float v = -0.5f + dv * 0.5f; // emit ray from pixel centers
    Ray ray;
    for (int h = 0; h < YRES; h++, v += dv) {
        float u = -0.5f + du * 0.5f;
        for (int w = 0; w < XRES; w++, u += du) {
            Hit hit;
            clearHit(&hit);
            createRay(u, v, &ray);
            for (int s = 0; s < n; s++) {
                if (shapes[s]->op.intersect(shapes[s], &ray, &hit.t)) {
                    hit.best = shapes[s];
                }
            }
            fputc(hit.best ? '.' : ' ', stdout);
        }
        fputc('\n', stdout);
    }
}



