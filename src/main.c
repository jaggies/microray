/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include "sphere.h"
#include "pointlight.h"
#include "hit.h"

#define XRES 80
#define YRES (XRES/2)
#define MAXSHAPES 10
#define MAXLIGHTS 8

typedef struct World {
    Shape* shapes[MAXSHAPES];
    Light* lights[MAXLIGHTS];
    int nShapes;
    int nLights;
} World;

World* createWorld() {
    World* world = (World*) malloc(sizeof(World));
    world->nShapes = 0;
    world->nLights = 0;
    world->shapes[world->nShapes++] = createSphere(0, 0, 0, 0.5);
    Vec3 point, color;
    makeVec3(5,5,5,&point);
    makeVec3(1,1,1, &color);
    world->lights[world->nLights++] = createPointLight(&point, &color);
    return world;
}

void createRay(float u, float v, Ray* ray) {
    makeVec3(u, v, 1, &ray->point);
    makeVec3(0, 0, -1, &ray->dir);
    normalize(&ray->dir);
}

Vec3* shade(Ray* ray, World* world, Hit* hit, Vec3* color) {
    if (hit->best) {
        mult(&ray->dir, hit->t, &hit->point);
        add(&hit->point, &ray->point, &hit->point);
        hit->best->op.normal(hit->best, ray, hit, &hit->normal);
        for (int i = 0; i < world->nLights; i++) {
            Light* light = world->lights[i];
            Ray lightRay;
            light->op.makeRay(light, &hit->point, &lightRay);
            float kd = dot(&lightRay.dir, &hit->normal);
            makeVec3(1, 1, 1, color);
            mult(color, kd > 0 ? kd : 0, color);
        }
    } else {
        makeVec3(0, 0, 0, color);
    }
    return color;
}

void trace(Ray* ray, World* world, Vec3* color) {
    Hit hit;
    clearHit(&hit);
    for (int s = 0; s < world->nShapes; s++) {
        Shape* shape = world->shapes[s];
        if (shape->op.intersect(shape, ray, &hit.t)) {
            hit.best = shape;
        }
    }
    shade(ray, world, &hit, color);
}

int main(int argc, char **argv)
{
    int n = 0;
    World* world = createWorld();
    float du = 1.0f / XRES, dv = 1.0f / YRES;
    float v = -0.5f + dv * 0.5f; // emit ray from pixel centers
    Ray ray;
    for (int h = 0; h < YRES; h++, v += dv) {
        float u = -0.5f + du * 0.5f;
        for (int w = 0; w < XRES; w++, u += du) {
            createRay(u, v, &ray);
            Vec3 color;
            trace(&ray, world, &color);
            fputc('0' + color.x * 10, stdout);
        }
        fputc('\n', stdout);
    }
}



