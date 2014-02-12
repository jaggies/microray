/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include "sphere.h"
#include "triangle.h"
#include "pointlight.h"
#include "hit.h"
#include "phongshader.h"
#include "perspectivecamera.h"

#define XRES 1024
#define YRES XRES
#define MAXSHAPES 10
#define MAXLIGHTS 8
#define MAXDEPTH 2 // max number of reflected rays
#define RAY_EPSILON 1.0e-5f

typedef struct World {
    Shape* shapes[MAXSHAPES];
    Light* lights[MAXLIGHTS];
    Camera* camera;
    int nShapes;
    int nLights;
    Vec3 background; // background colors
    float epsilon; // intersection slop
} World;

extern Camera* makeCamera();

World* createWorld() {
    World* world = (World*) malloc(sizeof(World));
    world->epsilon = RAY_EPSILON;
    world->nShapes = 0;
    world->nLights = 0;
    world->camera = makeCamera();
    Vec3 diffuse, specular, ambient;
    makeVec3(0.5,0.5,0.5,&specular);
    makeVec3(0.1,0.1,0.1,&ambient);
    makeVec3(0.5,0.0,0.0,&diffuse);
    Shader* red = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.4f, 1.0f, 0.5f);
    makeVec3(0.0,0.0,0.5,&diffuse);
    Shader* blu = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.4f, 1.0f, 0.5f);
    Shader* mirror = createPhongShader(&diffuse, &specular, &ambient, 20.0f, 1.4, 1.0f, 0.0f);
    world->shapes[world->nShapes++] = createSphere(0.25, 0, 0, 0.25, red);
    world->shapes[world->nShapes++] = createSphere(-0.25, 0, 0, 0.25, blu);
    float plane[][3] = { {-1, -0.25, -1}, {1, -0.25, -1}, {1, -0.25, 1}, {-1, -0.25, 1} };
    world->shapes[world->nShapes++] = createTriangle((Vec3*) plane[0], (Vec3*) plane[1],
            (Vec3*) plane[2], mirror);
    world->shapes[world->nShapes++] = createTriangle((Vec3*) plane[2], (Vec3*) plane[3],
            (Vec3*) plane[0], mirror);
    Vec3 point, color;
    makeVec3(5,5,5, &point);
    makeVec3(1,1,1, &color);
    world->lights[world->nLights++] = createPointLight(&point, &color);
    makeVec3(0.2, 0.3, 0.7, &world->background);
    return world;
}

Camera* makeCamera() {
    Vec3 from; makeVec3(1, 1, 1, &from);
    Vec3 at; makeVec3(0, 0, 0, &at);
    Vec3 up; makeVec3(0, 1, 0, &up);
    float aspect =  (float) XRES / YRES;
    return (Camera*) createPerspectiveCamera(&from, &at, &up, 45.0f, aspect);
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
            // nudge the point to avoid self-intersection
            addscaled(&reflected.point, world->epsilon, &reflected.dir, &reflected.point);
            negate(&reflected.dir); // TODO: is this correct?
            trace(&reflected, world, &reflectColor, maxdepth - 1);
            mult(&reflectColor, kr, &reflectColor);
            add(&reflectColor, color, color);
        }
    } else {
        copy(&world->background, color);
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
    World* world = createWorld();
    float du = 1.0f / XRES, dv = 1.0f / YRES;
    float v = 0.0f + dv * 0.5f; // emit ray from pixel centers
    for (int h = 0; h < YRES; h++, v += dv) {
        float u = 0.0f + du * 0.5f;
        for (int w = 0; w < XRES; w++, u += du) {
            Ray ray;
            Vec3 color;
            world->camera->op.makeRay(world->camera, u, 1.0f-v, &ray);
            trace(&ray, world, &color, MAXDEPTH);
            putPixel(w, h, &color);
        }
    }
}



