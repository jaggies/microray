/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include "sphere.h"
#include "pointlight.h"
#include "hit.h"
#include "phongshader.h"

#define XRES 1024
#define YRES XRES
#define MAXSHAPES 10
#define MAXLIGHTS 8
#define MAXDEPTH 1 // max number of reflected rays

typedef struct World {
    Shape* shapes[MAXSHAPES];
    Light* lights[MAXLIGHTS];
    int nShapes;
    int nLights;
} World;

World* createWorld() {
    World* world = (World*) malloc(sizeof(World));
    Vec3 diffuse, specular, ambient;
    makeVec3(0.5,0.5,0.5,&specular);
    makeVec3(0.1,0.1,0.1,&ambient);
    makeVec3(0.5,0.0,0.0,&diffuse);
    Shader* red = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.4f, 0.5f, 0.5f);
    makeVec3(0.0,0.0,0.5,&diffuse);
    Shader* blu = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.4f, 0.5f, 0.5f);
    world->nShapes = 0;
    world->nLights = 0;
    world->shapes[world->nShapes++] = createSphere(0.25, 0, 0, 0.25, red);
    world->shapes[world->nShapes++] = createSphere(-0.25, 0, 0, 0.25, blu);
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

extern void trace(Ray* ray, World* world, Vec3* color, int maxdepth);

Vec3* shade(Ray* ray, World* world, Hit* hit, Vec3* color, int maxdepth) {
    if (hit->best) {
        mult(&ray->dir, hit->t, &hit->point);
        add(&hit->point, &ray->point, &hit->point);
        hit->best->op.normal(hit->best, ray, hit, &hit->normal);
        reflectionDirection(&ray->dir, &hit->normal, &hit->reflect);
        makeVec3(0,0,0, color);
        Shader* shader = hit->best->op.shader;
        for (int i = 0; i < world->nLights; i++) {
            Vec3 tmpColor;
            Light* light = world->lights[i];
            light->op.makeRay(light, &hit->point, &hit->lightRay);
            shader->op.evaluate(shader, hit, &tmpColor);
            add(&tmpColor, color, color);
        }
        float kr = shader->op.getReflectionAmount(shader);
        if (kr > 0.0f && maxdepth) {
            Vec3 reflectColor;
            Ray reflected;
            copy(&hit->point, &reflected.point);
            copy(&hit->reflect, &reflected.dir);
            negate(&reflected.dir); // TODO: is this correct?
            trace(&reflected, world, &reflectColor, maxdepth - 1);
            mult(&reflectColor, kr, &reflectColor);
            add(&reflectColor, color, color);
        }
    } else {
        makeVec3(0, 0, 0, color);
    }
    return color;
}

void trace(Ray* ray, World* world, Vec3* color, int maxdepth) {
    Hit hit;
    clearHit(&hit);
    for (int s = 0; s < world->nShapes; s++) {
        Shape* shape = world->shapes[s];
        if (shape->op.intersect(shape, ray, &hit.t)) {
            hit.best = shape;
        }
    }
    shade(ray, world, &hit, color, maxdepth);
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

void putPixel(int x, int y, Vec3* color) {
    static FILE *fp = 0;
    if (!fp) {
        fp = fopen("out.ppm", "w");
        fprintf(fp, "P6 %d %d 255\n", XRES, YRES);
        assert(fp != 0);
    }
    int r = min(255, max(0, round(color->x * 255)));
    int g = min(255, max(0, round(color->y * 255)));
    int b = min(255, max(0, round(color->z * 255)));
    fputc(r, fp);
    fputc(g, fp);
    fputc(b, fp);
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
            trace(&ray, world, &color, MAXDEPTH);
            putPixel(w, h, &color);
        }
    }
}



