/*
 * showpbm.cpp
 *
 *  Created on: Oct 13, 2018
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef DOS
#include <conio.h> // kbhit()
#endif
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
static uint8_t* buffer;
NetPBM* pbm;

static void showGray(void* clientData, int x, int y, unsigned char pixel[3]) {
    Vesa* vesa = (Vesa*) clientData;
    if (y != oldy) {
        vesa->moveTo(offsetX, offsetY + oldy);
        vesa->color(pixel[0]);
        vesa->span(buffer, pbm->width);
        idx = 0;
        oldy = y;
        if (kbhit() && (getch() == 27)) {
            exit(0);
        }
    }
    buffer[idx++] = pixel[0];
}

static void showRGB(void* clientData, int x, int y, unsigned char pixel[3]) {
    Vesa* vesa = (Vesa*) clientData;
    if (y != oldy) {
        vesa->moveTo(offsetX, offsetY + oldy);
        vesa->color(pixel[0]);
        vesa->span(buffer, pbm->width);
        idx = 0;
        oldy = y;
        if (kbhit() && (getch() == 27)) {
            exit(0);
        }
    }
    buffer[idx++] = dither(RBITS, GBITS, BBITS, x, y, pixel[0], pixel[1], pixel[2]);
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
    printf("Loading %s: %dx%d pixels, mode=%d\n", argv[1], pbm->width, pbm->height, pbm->mode);

    buffer = new uint8_t[pbm->width];

    if (!buffer) {
        printf("Couldn't allocate line buffer!!\n");
        return 0;
    }

    if (!vesa.setMode(pbm->width, pbm->height, 8)) {
        fprintf(stderr, "Couldn't find a suitable video mode for %dx%d\n", pbm->width, pbm->height);
        return 0;
    }

    vesa.color(0);
    vesa.clear();

    offsetX = (vesa.width() - pbm->width) / 2;
    offsetY = (vesa.height() - pbm->height) / 2;

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
    while (getch() != 27)
        ;
    return 1;
}


