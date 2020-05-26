/*
 * main.cpp
 *
 *  Created on: Mar 14, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "os.h"
#include "io.h"
#include "hd63484.h"
#include "imagraph.h"
#include "util.h"
#include "hit.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "pointlit.h"
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
#define RBITS 3 // TODO: set up palette in this code
#define GBITS 3
#define BBITS 2

static void makePalette(int rbits, int gbits, int bbits) {
    int index = 0;
    for (index = 0; index < 256; index++) {
        int r = (index >> (gbits + bbits)) << (8-rbits);
        int g = (index >> (bbits)) << (8-gbits);
        int b = (index) << (8-bbits);
        palette(index, r, g, b);
    }
}

// This creates a brighter palette by using lower bits. Needs more testing.
static void makePalette1(int rbits, int gbits, int bbits) {
    const int rlevels = 1 << rbits;
    const int glevels = 1 << gbits;
    const int blevels = 1 << bbits;
    int index = 0;
    int r, g, b;
    for (r = 0; r < rlevels; r++) {
        uint16_t red = 0xff * r / (rlevels - 1);
        for (g = 0; g < glevels; g++) {
            uint16_t green = 0xff * g / (glevels - 1);
            for (b = 0; b < blevels; b++) {
                uint16_t blue = 0xff * b / (blevels - 1);
                palette(index++, red, green, blue);
            }
        }
    }
}

static bool pixel(uint16_t x, uint16_t y, uint8_t* rgb, void* userdata) {
    uint8_t index;
    #ifdef USE_ERROR_DIFFUSION
    int rx = diffusion_dither(1<<8, 1<<RBITS, rgb[0], &rerr);
    int gx = diffusion_dither(1<<8, 1<<GBITS, rgb[1], &gerr);
    int bx = diffusion_dither(1<<8, 1<<BBITS, rgb[2], &berr);
    #else
    uint8_t rx = ordered_dither(1<<8, 1<<RBITS, x, y, rgb[0]);
    uint8_t gx = ordered_dither(1<<8, 1<<GBITS, x, y, rgb[1]);
    uint8_t bx = ordered_dither(1<<8, 1<<BBITS, x, y, rgb[2]);
    #endif
    index = (rx << (GBITS + BBITS)) | (gx << BBITS) | bx;
    dot(x, 1024-y, index);
    pbm->write(pbm, rgb);
    return true;
}

static void renderToFile(World* world, const char* outpath)
{
    pbm = createNetPBM(outpath);

    if (pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        renderImage(world, pixel, NULL);
        pbm->close(pbm);
    } else {
        printf("Can't write image '%s'\n", outpath);
        return;
    }
}

int main(int argc, char **argv)
{
    World* world;
    char* outpath;
    board_init();
    makePalette(RBITS, GBITS, BBITS);

    printf("*** Imagraph Renderer v1.0 ***\n");

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
    outpath = getImagePath(argv[1]);
    renderToFile(world, basename(outpath));
    destroyWorld(world);
    free(outpath);

#ifdef PROFILE
    printf("%ld intersections\n", intersections);
#endif /* PROFILE */
    return 0;
}
