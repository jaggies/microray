/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include "os.h"
#include "hit.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "pointlit.h"
#include "phongshd.h"
#include "checkshd.h"
#include "perspcam.h"
#include "world.h"

static const float plane[][3] = {
	{-1.0f, -0.25f, -1.0f}, {1.0f, -0.25f, -1.0f}, {1.0f, -0.25f, 1.0f}, {-1.0f, -0.25f, 1.0f} };
static const float uv[][2] = { {0.0f,0.0f}, {1.0f,0.0f}, {1.0f,1.0f}, {0.0f,1.0f} };

Camera* makeCamera(int xres, int yres) {
    float aspect =  (float) xres / yres;
    Vec3 from, at, up;
    vec3(-1, 1, 1, &from);
    vec3(0, 0, 0, &at);
    vec3(0, 1, 0, &up);
    return (Camera*) createPerspectiveCamera(&from, &at, &up, 45.0f, aspect);
}

World* testLoad(int xres, int yres) {
    World* world = createWorld();
    Vec3 diffuse, specular, ambient, point, color;
    Vec2 scale, bias;
    Shader* red;
    Shader* blu;
    Shader* odd;
    Shader* even;
    Shader* checker;

    world->camera = makeCamera(xres, yres);
    vec3(0.5f,0.5f,0.5f,&specular);
    vec3(0.0f,0.0f,0.0f,&ambient);
    vec3(0.0f,0.0f,0.0f,&diffuse);
    red = (Shader*) createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.1f, 0.5f, 0.5f);
    vec3(0.0f,0.0f,0.0f,&diffuse);
    blu = (Shader*) createPhongShader(&diffuse,  &specular,  &ambient, 20.0f, 1.1f, 0.5f, 0.5f);
    vec3(0.5f,0.5f,0.5f,&diffuse);
    /* Shader* mirror */ (void) createPhongShader(&diffuse, &specular, &ambient, 10.0f, 1.4f, 0.5f, 0.0f);
    vec3(1.0f,0.0f,0.0f, &diffuse);
    odd = (Shader*) createPhongShader(&diffuse, &specular, &ambient, 10.0f, 1.4f, 0.5f, 0.0f);
    vec3(0.0f,1.0f,0.0f, &diffuse);
    even = (Shader*) createPhongShader(&diffuse, &specular, &ambient, 10.0f, 1.4f, 0.5f, 0.0f);
    vec2(10.0f,10.0f,&scale);
    vec2(0.0f,0.0f,&bias);
    checker = (Shader*) createCheckerboardShader(odd, even, &scale, &bias);
    world->shapes[world->nShapes++] = createSphere(0.25f, 0.0f, 0.0f, 0.25f, red);
    world->shapes[world->nShapes++] = createSphere(-0.25f, 0.0f, 0.0f, 0.25f, blu);
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
    vec3(0.2f, 0.3f, 0.7f, &world->background);
    return world;
}

