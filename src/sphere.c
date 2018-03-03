/*
 * sphere.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include "sphere.h"
#include "shader.h"
#include "hit.h"

static float tmin = 0.0f;

static
int intersect(struct Shape* shape, Ray* ray, float *tmax) {
    Sphere* sphere = (Sphere*) shape;

    // Distance from center to perpendicular plane
    Vec3 dir;
    sub3(&sphere->position, &ray->point, &dir);
    float b = dot3(&ray->dir, &dir);
    float disc = b*b - dot3(&dir, &dir) + sphere->radius2;

    int hit = 0;
    if (disc >= 0.0f) {
        disc = sqrt(disc);
        float t2 = b + disc;   // farthest point
        if (t2 > tmin && t2 < *tmax) {
            *tmax = t2;
            hit++;
        }
        float t1 = b - disc;   // nearest point
        if (t1 > tmin && t1 < *tmax) {
            *tmax = t1;
            hit++;
        }
    }
    return hit;
}

static
void normal(struct Shape* shape, Hit* hit, Vec3 *n) {
    Sphere* sphere = (Sphere*) shape;
    sub3(&hit->point, &sphere->position, n);
    normalize3(n);
}

static
void uv(struct Shape* shape, struct Hit* hit, Vec2 * uv) {
    uv->x = uv->y = 0.0f; // TODO
}

static
void bounds(struct Shape* shape, Vec3* min, Vec3* max) {
    Sphere* sphere = (Sphere*) shape;
    min->x = sphere->position.x - sphere->radius;
    min->y = sphere->position.y - sphere->radius;
    min->z = sphere->position.z - sphere->radius;
    max->x = sphere->position.x + sphere->radius;
    max->y = sphere->position.y + sphere->radius;
    max->z = sphere->position.z + sphere->radius;
}

static ShapeOps _sphereOps;

Shape* createSphere(float x, float y, float z, float r, Shader* shader) {
    Sphere* sphere = (Sphere*) malloc(sizeof(Sphere));
    if (!_sphereOps.intersect) {
        _sphereOps.intersect = intersect;
        _sphereOps.normal = normal;
        _sphereOps.uv = uv;
        _sphereOps.bounds = bounds;
    }
    sphere->op = &_sphereOps;
    sphere->shader = shader;
    sphere->position.x = x;
    sphere->position.y = y;
    sphere->position.z = z;
    sphere->radius = r;
    sphere->radius2 = r * r;
    return (Shape*) sphere;
}
