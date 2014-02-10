/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include "sphere.h"

#define XRES 20
#define YRES 10

void createRay(float u, float v, Ray* ray) {
    ray->point.x = u;
    ray->point.y = v;
    ray->point.z = 1;
    ray->dir.x = 0;
    ray->dir.y = 0;
    ray->dir.z = -1.0f;
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
            float t = 1.0e7;
            createRay(u, v, &ray);
            int hit = 0;
            for (int s = 0; s < n; s++) {
                if (shapes[s]->op.intersect(shapes[s], &ray, &t)) {
                    hit++;
                }
            }
            fputc(hit ? '.' : ' ', stdout);
        }
        fputc('\n', stdout);
    }
}



