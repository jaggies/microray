/*
 * vesaren.cpp
 *
 *  Created on: Mar 29, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "os.h"
#include "io.h"
#include "util.h"

extern "C" {
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
#include "dither.h"
};

#include "vesa.h"

#ifdef PROFILE
long intersections = 0;
#endif /* PROFILE */

static int centerx, centery;

static NetPBM* pbm = 0;
#define RBITS 3 // TODO: set up palette in this code
#define GBITS 3
#define BBITS 2

// This creates a brighter palette by using lower bits. Needs more testing.
static void makePalette(Vesa& vesa, int rbits, int gbits, int bbits) {
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
                vesa.palette(index++, red, green, blue);
            }
        }
    }
}

static void pixel(uint16_t x, uint16_t y, uint8_t* rgb, void* userData) {
    Vesa* vesa = (Vesa*) userData;
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
    vesa->color(index);
    vesa->moveTo(centerx + x, centery + y);
    vesa->dot();
    pbm->write(pbm, rgb);
}

static void renderToFile(World* world, Vesa* vesa, const char* outpath)
{
    pbm = createNetPBM(outpath);

    // Pick nearest VESA mode to scene resolution
    if (!vesa->setMode(world->width, world->height, 8)) {
        printf("Image too large to set graphics mode\n");
        return;
    }

    centerx = (vesa->width() - world->width) / 2;
    centery = (vesa->height() - world->height) / 2;

    makePalette(*vesa, RBITS, GBITS, BBITS);

    if (pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        renderImage(world, pixel, vesa);
        pbm->close(pbm);
    } else {
        printf("Can't write image '%s'\n", outpath);
        return;
    }
}

int main(int argc, char **argv)
{
    const char* outpath = "out.ppm"; /* TODO: get path from arguments */
    World* world;
    Vesa vesa;

    printf("*** VESA Renderer v1.0 ***\n");

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

    renderToFile(world, &vesa, outpath);

#ifdef PROFILE
    printf("%ld intersections\n", intersections);
#endif /* PROFILE */
    return 0;
}



