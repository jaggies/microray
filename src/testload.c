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
    float aspect =  (float) xres / yres;
    Vec3 from, at, up;
    vec3(-1, 1, 1, &from);
    vec3(0, 0, 0, &at);
    vec3(0, 1, 0, &up);
    return (Camera*) createPerspectiveCamera(&from, &at, &up, 45.0f, aspect);
}

World* testLoad(int xres, int yres) {
    static const float plane[][3] = { {-1, -0.25, -1}, {1, -0.25, -1}, {1, -0.25, 1}, {-1, -0.25, 1} };
    static const float uv[][2] = { {0,0}, {1,0}, {1,1}, {0,1} };
    World* world = createWorld();
    Vec3 diffuse, specular, ambient, point, color;
    Vec2 scale, bias;
    Shader* red;
    Shader* blu;
    Shader* odd;
    Shader* even;
    Shader* checker;

    world->camera = makeCamera(xres, yres);
    vec3(0.5,0.5,0.5,&specular);
    vec3(0.0,0.0,0.0,&ambient);
    vec3(0.0,0.0,0.0,&diffuse);
    red = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.1f, 0.5f, 0.5f);
    vec3(0.0,0.0,0.0,&diffuse);
    blu = createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.1f, 0.5f, 0.5f);
    vec3(0.5,0.5,0.5,&diffuse);
    /* Shader* mirror */ (void) createPhongShader(&diffuse, &specular, &ambient, 10.0f, 1.4, 0.5f, 0.0f);
    vec3(1.0,0.0,0.0, &diffuse);
    odd = createPhongShader(&diffuse, &specular, &ambient, 10.0f, 1.4, 0.5f, 0.0f);
    vec3(0.0,1.0,0.0, &diffuse);
    even = createPhongShader(&diffuse, &specular, &ambient, 10.0f, 1.4, 0.5f, 0.0f);
    vec2(10,10,&scale);
    vec2(0,0,&bias);
    checker = createCheckerboardShader(odd, even, &scale, &bias);
    world->shapes[world->nShapes++] = createSphere(0.25, 0, 0, 0.25, red);
    world->shapes[world->nShapes++] = createSphere(-0.25, 0, 0, 0.25, blu);
    world->shapes[world->nShapes++] = createTriangle(
            (Vec3*) plane[0], (Vec3*) plane[1], (Vec3*) plane[2],
            (Vec2*) uv[0], (Vec2*) uv[1], (Vec2*) uv[2],
            checker);
    world->shapes[world->nShapes++] = createTriangle(
            (Vec3*) plane[2], (Vec3*) plane[3], (Vec3*) plane[0],
            (Vec2*) uv[2], (Vec2*) uv[3], (Vec2*) uv[0],
            checker);
    vec3(5,5,5, &point);
    vec3(1,1,1, &color);
    world->lights[world->nLights++] = createPointLight(&point, &color);
    vec3(0.2, 0.3, 0.7, &world->background);
    return world;
}

