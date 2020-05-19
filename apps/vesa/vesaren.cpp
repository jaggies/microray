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
#ifdef DOS
#include <conio.h>
#endif
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
#include "vesautil.h"

#ifdef PROFILE
long intersections = 0;
#endif /* PROFILE */

static int idx;
static int oldy;
static int offsetX;
static int offsetY;
static void* buffer;

static NetPBM* pbm = 0;
#define RBITS 3 // TODO: set up palette in this code
#define GBITS 3
#define BBITS 2

static bool showRGB(uint16_t x, uint16_t y, uint8_t* pixel, void* clientData) {
    Vesa* vesa = (Vesa*) clientData;
    if (y != oldy) {
        vesa->moveTo(offsetX, offsetY + oldy);
        // Hack. Add 1 to depth to round to the nearest byte, i.e. 15 -> 2, 16-> 2, etc.
        vesa->span(buffer, (1 + vesa->getDepth()) / 8, pbm->width);
        idx = 0;
        oldy = y;
        if (checkforkey(27)) return false; // don't continue
    }
    switch (vesa->getDepth()) {
        case 8:
            ((uint8_t*) buffer)[idx++] = dither(RBITS, GBITS, BBITS, x, y, pixel[0], pixel[1], pixel[2]);
        break;

        case 15:
        case 16:
            ((uint16_t*) buffer)[idx++] = vesa->color(pixel[0], pixel[1], pixel[2]);
        break;

        case 24:
            ((uint8_t*) buffer)[idx++] = pixel[0];
            ((uint8_t*) buffer)[idx++] = pixel[1];
            ((uint8_t*) buffer)[idx++] = pixel[2];
        break;

        case 32:
            ((uint8_t*) buffer)[idx++] = pixel[2];
            ((uint8_t*) buffer)[idx++] = pixel[1];
            ((uint8_t*) buffer)[idx++] = pixel[0];
            ((uint8_t*) buffer)[idx++] = 0;
        break;
    }
    pbm->write(pbm, pixel);
    return true;
}

static void renderToFile(World* world, Vesa* vesa, const char* outpath)
{
    pbm = createNetPBM();

    // Depths ranked by quality
    const uint8_t depths[] = {32, 24, 16, 15, 8};

    bool found = false;
    for (int i = 0; i < Number(depths); i++) {
        if (vesa->setMode(world->width, world->height, depths[i], Vesa::Any)) {
            found = true;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Couldn't find a suitable video mode for %dx%d\n", world->width, world->height);
        return;
    }

    offsetX = (vesa->width() - world->width) / 2;
    offsetY = (vesa->height() - world->height) / 2;

    if (pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        if (vesa->getDepth() == 8) {
            vesa->color(0111);
            makeDitherPalette(*vesa, RBITS, GBITS, BBITS);
        } else {
            vesa->color(20,20,20);
        }
        vesa->clear();
        uint32_t bufferSize = (uint32_t)(pbm->width) * (vesa->getDepth() + 1) / 8;
        buffer = new uint8_t[bufferSize];
        if (!buffer) {
            printf("Couldn't allocate line buffer (size=%d)!!\n", bufferSize);
        } else {
            renderImage(world, showRGB, vesa);
        }
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



