/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include "sphere.h"
#include "triangle.h"
#include "pointlight.h"
#include "hit.h"
#include "phongshader.h"
#include "checkerboardshader.h"
#include "perspectivecamera.h"
#include "world.h"

Camera* makeCamera(int xres, int yres) {
    Vec3 from; vec3(-1, 1, 1, &from);
    Vec3 at; vec3(0, 0, 0, &at);
    Vec3 up; vec3(0, 1, 0, &up);
    float aspect =  (float) xres / yres;
    return (Camera*) createPerspectiveCamera(&from, &at, &up, 45.0f, aspect);
}

World* testLoad(int xres, int yres) {
    World* world = createWorld();
    world->camera = makeCamera(xres, yres);
    Vec3 diffuse, specular, ambient;
    vec3(0.5,0.5,0.5,&specular);
    vec3(0.0,0.0,0.0,&ambient);
    vec3(0.0,0.0,0.0,&diffuse);
    Shader* red = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.1f, 0.5f, 0.5f);
    vec3(0.0,0.0,0.0,&diffuse);
    Shader* blu = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.1f, 0.5f, 0.5f);
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

