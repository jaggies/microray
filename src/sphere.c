/*
 * sphere.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include "sphere.h"
#include "shader.h"
#include "hit.h"

static
int intersect(struct Shape* shape, Ray* ray, float *tmax) {
    Sphere* sphere = (Sphere*) shape;

    // Distance from center to perpendicular plane
    Vec3 dir;
    sub(&sphere->position, &ray->point, &dir);
    float b = dot(&ray->dir, &dir);
    float disc = b*b - dot(&dir, &dir) + sphere->radius2;

    int hit = 0;
    if (disc >= 0.0f) {
        disc = sqrt(disc);
        float t2 = b + disc;   // farthest point
        if (t2 < *tmax) {
            *tmax = t2;
            hit++;
        }
        float t1 = b - disc;   // nearest point
        if (t1 < *tmax) {
            *tmax = t1;
            hit++;
        }
    }
    return hit;
}

static
void normal(struct Shape* shape, Ray* ray, Hit* hit, Vec3 *n) {
    Sphere* sphere = (Sphere*) shape;
    sub(&hit->point, &sphere->position, n);
    normalize(n);
}

Shape* createSphere(float x, float y, float z, float r, Shader* shader) {
    Sphere* sphere = (Sphere*) malloc(sizeof(Sphere));
    sphere->op.intersect = intersect;
    sphere->op.normal = normal;
    sphere->op.shader = shader;
    sphere->position.x = x;
    sphere->position.y = y;
    sphere->position.z = z;
    sphere->radius2 = r * r;
    return (Shape*) sphere;
}
