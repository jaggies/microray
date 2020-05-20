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

#define tmin (0.0f)

#ifdef PROFILE
extern long intersections;
#endif /* PROFILE */

static
int sphereIntersect(Shape* shape, Ray* ray, Hit* hit) {
    Vec3 dir;
    float b, disc;
    int result;
    Sphere* sphere;

    if(shape == hit->ignore)
        return 0;

#ifdef PROFILE
    intersections++;
#endif /* PROFILE */

    sphere = (Sphere*) shape;

    /* Distance from center to perpendicular plane */
    sub3(&sphere->position, &ray->point, &dir);
    b = dot3(&ray->dir, &dir);
    disc = b*b - dot3(&dir, &dir) + sphere->radius2;

    result = 0;
    if (disc >= 0.0f) {
        float t1, t2;
        disc = sqrt(disc);
        t2 = b + disc;   /* farthest point */
        if (t2 > tmin && t2 < hit->t) {
            hit->best = shape;
            hit->t = t2;
            result++;
        }
        t1 = b - disc;   /* nearest point */
        if (t1 > tmin && t1 < hit->t) {
            hit->best = shape;
            hit->t = t1;
            result++;
        }
    }
    return result;
}

static
void sphereNormal(Shape* shape, Hit* hit, Vec3 *n) {
    Sphere* sphere = (Sphere*) shape;
    sub3(&hit->point, &sphere->position, n);
    normalize3(n);
}

static
void sphereUV(Shape* shape, Hit* hit, Vec2 * uv) {
    uv->x = uv->y = 0.0f; /* TODO */
}

static
void sphereBounds(Shape* shape, Vec3* min, Vec3* max) {
    Sphere* sphere = (Sphere*) shape;
    min->x = sphere->position.x - sphere->radius;
    min->y = sphere->position.y - sphere->radius;
    min->z = sphere->position.z - sphere->radius;
    max->x = sphere->position.x + sphere->radius;
    max->y = sphere->position.y + sphere->radius;
    max->z = sphere->position.z + sphere->radius;
}

static
void sphereDestroy(Shape* shape) {
    Sphere* sphere = (Sphere*) shape;
    free(sphere);
}

static ShapeOps _sphereOps;

Shape* createSphere(float x, float y, float z, float r, Shader* shader) {
    Sphere* sphere = (Sphere*) malloc(sizeof(Sphere));
    if (!_sphereOps.intersect) {
        _sphereOps.intersect = sphereIntersect;
        _sphereOps.normal = sphereNormal;
        _sphereOps.uv = sphereUV;
        _sphereOps.bounds = sphereBounds;
        _sphereOps.destroy = sphereDestroy;
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
