/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include "os.h"
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

static bool pixel(uint16_t x, uint16_t y, uint8_t* rgb, void* userdata) {
    NetPBM* pbm = (NetPBM*) userdata;
    pbm->write(pbm, rgb);
    if (x == 0) {
        printf("Line %d\n", y);
    }
    return true;
}

static void renderToFile(World* world, const char* outpath)
{
    NetPBM* pbm = createNetPBM(outpath);
    if (pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        printf("Rendering scene (%dx%d)\n", world->width, world->height);
        renderImage(world, pixel, pbm);
        pbm->close(pbm);
    } else {
        printf("Can't write image '%s'\n", outpath);
        return;
    }
    destroyPBM(pbm);
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
    destroyWorld(world);

#ifdef PROFILE
    printf("%ld intersections\n", intersections);
#endif /* PROFILE */
    return 0;
}

