/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include "util.h"
#include "sphere.h"
#include "triangle.h"
#include "pointlit.h"
#include "hit.h"
#include "phongshd.h"
#include "checkshd.h"
#include "perspcam.h"
#include "world.h"
#include "raytrace.h"
#include "loader.h"
#include "netpbm.h"
#include "testload.h"

#ifdef PROFILE
long intersections = 0;
#endif /* PROFILE */

static NetPBM* pbm = 0;

static void pixel(uint16_t x, uint16_t y, uint8_t* rgb) {
    pbm->write(pbm, rgb);
}

static void renderToFile(World* world, const char* outpath)
{
    float du = 1.0f / world->width, dv = 1.0f / world->height;
    float v = 0.0f + dv * 0.5f; /* emit ray from pixel centers */
    pbm = createNetPBM(outpath);

    if (pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        printf("Rendering scene (%dx%d)\n", world->width, world->height);
        renderImage(world, pixel);
        pbm->close(pbm);
    } else {
        printf("Can't write image '%s'\n", outpath);
        return;
    }
}

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
    renderToFile(world, outpath);

#ifdef PROFILE
    printf("%ld intersections\n", intersections);
#endif /* PROFILE */
    return 0;
}

