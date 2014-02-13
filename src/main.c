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
#include "checkerboardshader.h"
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
    vec3(0.5,0.5,0.5,&specular);
    vec3(0.1,0.1,0.1,&ambient);
    vec3(0.5,0.0,0.0,&diffuse);
    Shader* red = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.4f, 0.5f, 0.5f);
    vec3(0.0,0.0,0.5,&diffuse);
    Shader* blu = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.4f, 0.5f, 0.5f);
    vec3(0.5,0.5,0.5,&diffuse);
    Shader* mirror = createPhongShader(&diffuse, &specular, &ambient, 10.0f, 1.4, 0.5f, 0.0f);
    Vec3 check1; vec3(1,0,0, &check1);
    Vec3 check2; vec3(0,1,0, &check2);
    Vec2 scale; vec2(10,10,&scale);
    Vec2 bias; vec2(0,0,&bias);
    Shader* checker = createCheckerboardShader(&check1, &check2, &scale, &bias, mirror, &((PhongShader*)mirror)->diffuse);
    world->shapes[world->nShapes++] = createSphere(0.25, 0, 0, 0.25, red);
    world->shapes[world->nShapes++] = createSphere(-0.25, 0, 0, 0.25, blu);
    float plane[][3] = { {-1, -0.25, -1}, {1, -0.25, -1}, {1, -0.25, 1}, {-1, -0.25, 1} };
    float uv[][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
    world->shapes[world->nShapes++] = createTriangle(
            (Vec3*) plane[0], (Vec3*) plane[1], (Vec3*) plane[2],
            (Vec2*) uv[0], (Vec2*) uv[1], (Vec2*) uv[2],
            checker);
    world->shapes[world->nShapes++] = createTriangle(
            (Vec3*) plane[2], (Vec3*) plane[3], (Vec3*) plane[0],
            (Vec2*) uv[2], (Vec2*) uv[3], (Vec2*) uv[0],
            checker);
    Vec3 point, color;
    vec3(5,5,5, &point);
    vec3(1,1,1, &color);
    world->lights[world->nLights++] = createPointLight(&point, &color);
    vec3(0.2, 0.3, 0.7, &world->background);
    return world;
}

Camera* makeCamera() {
    Vec3 from; vec3(-1, 1, 1, &from);
    Vec3 at; vec3(0, 0, 0, &at);
    Vec3 up; vec3(0, 1, 0, &up);
    float aspect =  (float) XRES / YRES;
    return (Camera*) createPerspectiveCamera(&from, &at, &up, 45.0f, aspect);
}

void createRay(float u, float v, Ray* ray) {
    vec3(u, v, 1, &ray->point);
    vec3(0, 0, -1, &ray->dir);
    normalize3(&ray->dir);
}

extern void trace(Ray* ray, World* world, Vec3* color, int maxdepth);
extern int shadow(Ray* ray, World* world);

Vec3* shade(Ray* ray, World* world, Hit* hit, Vec3* color, int maxdepth) {
    if (hit->best) {
        vec3(0,0,0, color);
        addscaled3(&ray->point, hit->t, &ray->dir, &hit->point);
        hit->best->op.normal(hit->best, hit, &hit->normal);
        hit->best->op.uv(hit->best, hit, &hit->uv);
        reflectionDirection(&ray->dir, &hit->normal, &hit->reflect);
        Shader* shader = hit->best->op.shader;
        for (int i = 0; i < world->nLights; i++) {
            Vec3 tmpColor;
            Light* light = world->lights[i];
            light->op.makeRay(light, &hit->point, &hit->lightRay);
            addscaled3(&hit->lightRay.point, world->epsilon, &hit->lightRay.dir, &hit->lightRay.point);
            hit->inShadow = shadow(&hit->lightRay, world);
            shader->op.evaluate(shader, hit, &tmpColor);
            add3(&tmpColor, color, color); // TODO: multiply by Light's ambient color
        }
        float kr = shader->op.getReflectionAmount(shader);
        if ((kr > 0.0f) && (maxdepth > 0)) {
            Vec3 reflectColor;
            Ray reflected;
            copy3(&hit->point, &reflected.point);
            copy3(&hit->reflect, &reflected.dir);
            // nudge the point to avoid self-intersection
            addscaled3(&reflected.point, world->epsilon, &reflected.dir, &reflected.point);
            trace(&reflected, world, &reflectColor, maxdepth - 1);
            mult3(&reflectColor, kr, &reflectColor);
            add3(&reflectColor, color, color);
        }
    } else {
        copy3(&world->background, color);
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

int shadow(Ray* ray, World* world) {
    Hit hit;
    clearHit(&hit);
    for (int s = 0; s < world->nShapes; s++) {
        Shape* shape = world->shapes[s];
        if (shape->op.intersect(shape, ray, &hit.t)) {
            return 1;
        }
    }
    return 0;
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



