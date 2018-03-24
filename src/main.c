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
#include "world.h"
#include "raytrace.h"
#include "loader.h"
#include "netpbm.h"
#include "testload.h"

#define MAXDEPTH 4 /* max number of reflected rays */

void renderImage(World* world, const char* outpath)
{
    int h, w;
    NetPBM* pbm;
    float du = 1.0f / world->width, dv = 1.0f / world->height;
    float v = 0.0f + dv * 0.5f; /* emit ray from pixel centers */
    pbm = createNetPBM(outpath, world->width, world->height);

    if (!pbm) {
        printf("Can't write image '%s'\n", outpath);
        return;
    }

    printf("Rendering scene (%dx%d)\n", world->width, world->height);
    for (h = 0; h < world->height; h++, v += dv) {
        float u = 0.0f + du * 0.5f;
        printf("Line %d (%d%%)\n", h, 100*h / world->height);
        for (w = 0; w < world->width; w++, u += du) {
            Ray ray;
            Vec3 color;
            world->camera->op->makeRay(world->camera, u, 1.0f-v, &ray);
            trace(&ray, 0 /* ignore */, world, &color, MAXDEPTH);
            pbm->putPixel(pbm, w, h, &color);
        }
    }
    pbm->close(pbm);
}

#ifdef PROFILE
long intersections = 0;
#endif /* PROFILE */

int main(int argc, char **argv)
{
    World* world;
    const char* outpath = "out.ppm"; /* TODO: get path from arguments */

    printf("*** MICRORAY ***\n");

    if (argc > 1) {
        printf("Loading %s\n", argv[1]);
        world = loadFile(argv[1]);
    } else {
        printf("Loading default scene\n");
        world = testLoad(100, 100);
    }
    if (world->nShapes == 0) {
        printf("World contains no shapes, exiting\n");
        return 0;
    }
    if (world->nLights == 0) {
        printf("World contains no lights, exiting\n");
        return 0;
    }
    if (!world->camera) {
        printf("World contains no camera, exiting\n");
        return 0;
    }
    renderImage(world, outpath);

#ifdef PROFILE
    printf("%ld intersections\n", intersections);
#endif /* PROFILE */
}

