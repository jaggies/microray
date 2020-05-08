/*
 * showpbm.cpp
 *
 *  Created on: Oct 13, 2018
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include "vesa.h"
#include "vesautil.h"
extern "C" {
#include "netpbm.h"
#include "dither.h"
}

#define RBITS 3
#define GBITS 3
#define BBITS 2

static int idx;
static int oldy;
static int offsetX;
static int offsetY;
static void* buffer;
NetPBM* pbm;

static bool showGray(void* clientData, int x, int y, unsigned char pixel[3]) {
    Vesa* vesa = (Vesa*) clientData;
    if (y != oldy) {
        vesa->moveTo(offsetX, offsetY + oldy);
        // Hack. Add 1 to depth to round to the nearest byte, i.e. 15 -> 2, 16-> 2, etc.
        vesa->span(buffer, (1 + vesa->getDepth()) / 8, pbm->width);
        idx = 0;
        oldy = y;
        if (checkforkey(27)) return false; // don't continue
    }
    if (vesa->getMemoryModel() == Vesa::DirectColor) {
        uint32_t color = vesa->color(pixel[0], pixel[0], pixel[0]);
        switch(vesa->getDepth()) {
            case 8:
                ((uint8_t*)buffer)[idx++] = color;
            break;

            case 15:
            case 16:
                ((uint16_t*)buffer)[idx++] = color;
            break;

            case 24:
                ((uint8_t*) buffer)[idx++] = pixel[0];
                ((uint8_t*) buffer)[idx++] = pixel[0];
                ((uint8_t*) buffer)[idx++] = pixel[0];
            break;

            case 32:
                ((uint8_t*) buffer)[idx++] = pixel[0];
                ((uint8_t*) buffer)[idx++] = pixel[0];
                ((uint8_t*) buffer)[idx++] = pixel[0];
                ((uint8_t*) buffer)[idx++] = pixel[0];
            break;
        }
    } else if (vesa->getMemoryModel() == Vesa::PackedPixel) {
        ((uint8_t*)buffer)[idx++] = pixel[0];
    }
    return true;
}

static bool showRGB(void* clientData, int x, int y, unsigned char pixel[3]) {
    Vesa* vesa = (Vesa*) clientData;
    if (y != oldy) {
        vesa->moveTo(offsetX, offsetY + oldy);
        vesa->color(pixel[0]);
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
    return true;
}

void usage(const char* name) {
    printf("Usage: %s <image.[ppm,pgm,pbm]>\n", name);
}

int main(int argc, char** argv) {
    Vesa vesa;
    pbm = createNetPBM();
    if (argc < 2) {
        usage(argv[0]);
        return 0;
    }
    int width, height, depth;
    if (!pbm->open(pbm, argv[1], &width, &height, &depth, NETPBM_READ)) {
        printf("Can't open %s\n", argv[1]);
        return 0;
    }

    // Depths ranked by quality
    const uint8_t depths[] = {32, 24, 16, 15, 8};

    bool found = false;
    for (int i = 0; i < Number(depths); i++) {
        if (vesa.setMode(pbm->width, pbm->height, depths[i], Vesa::Any)) {
            found = true;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Couldn't find a suitable video mode for %dx%d\n", pbm->width, pbm->height);
        return 0;
    }

    uint32_t bufferSize = (uint32_t)(pbm->width) * vesa.getDepth() / 8;
    buffer = new uint8_t[bufferSize];
    if (!buffer) {
        printf("Couldn't allocate line buffer (size=%d)!!\n", bufferSize);
        return 0;
    }

    vesa.color(0);
    vesa.clear();

    offsetX = (vesa.width() - pbm->width) / 2;
    offsetY = (vesa.height() - pbm->height) / 2;

    printf("Loading %s: %dx%d pixels, mode=%d\n", argv[1], pbm->width, pbm->height, pbm->mode);
    printf("Display %dx%d@%dbpp\n", vesa.width(), vesa.height(), vesa.getDepth());

    if (pbm->mode == 6) {
        makeDitherPalette(vesa, RBITS, GBITS, BBITS);
        pbm->read(pbm, showRGB, &vesa);
    } else if (pbm->mode == 5 || pbm->mode == 4) {
        makeGrayPalette(vesa, 0, 255);
        pbm->read(pbm, showGray, &vesa);
    } else {
        printf("Unknown mode: %d\n", pbm->mode);
        return 0;
    }
    while (!checkforkey(27))
        ;
    return 1;
}


