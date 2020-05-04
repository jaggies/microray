/*
 * testvesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "os.h"
#include "vesa.h"
#include "vesautil.h"

static int xres = 0;
static int yres = 0;
static int depth = 8;

static void makePalette(Vesa* vesa, int rbits, int gbits, int bbits) {
    int index = 0;
    for (index = 0; index < 256; index++) {
        int r = (index >> (gbits + bbits)) << (8-rbits);
        int g = (index >> (bbits)) << (8-gbits);
        int b = (index) << (8-bbits);
        vesa->palette(index, r, g, b);
    }
}

void drawCircles(Vesa* vesa, int n) {
    vesa->color(0x03); // blue
    vesa->clear();
    while(n--) {
        uint16_t color = rand() & 0xff;
        color |= color << 8;
        vesa->color(color);
        vesa->moveTo(rand() % xres, rand() % yres);
        vesa->circle(rand() % (yres/4), true);
        if (checkforkey(27)) return;
    }
}

void drawLines(Vesa* vesa, int n) {
    vesa->color(0x03); // blue
    vesa->clear();
    while (n--) {
        uint16_t color = rand() & 0xff;
        vesa->color(color);
        vesa->moveTo(rand() % xres, rand() % yres);
        vesa->lineTo(rand() % xres, rand() % yres);
        if (checkforkey(27)) return;
    }
}

void drawRectangles(Vesa* vesa, int n) {
    vesa->color(0x03); // blue
    vesa->clear();
    while(n--) {
        uint16_t color = rand() & 0xff;
        vesa->color(color);
        vesa->moveTo(rand() % xres, rand() % yres);
        vesa->rectangle(rand() % xres, rand() % yres, true);
        if (checkforkey(27)) return;
    }
}

void drawNgon(Vesa* vesa, int n) {
    vesa->color(0x03); // blue
    vesa->clear();
    uint16_t * x = new uint16_t[n];
    uint16_t * y = new uint16_t[n];

    for (int i = 0; i < n; i++) {
        float alpha = 2.0f*M_PI*i / (n-1);
        x[i] = cos(alpha) * yres/2 + xres/2;
        y[i] = sin(alpha) * yres/2 + yres/2;
    }
    vesa->color(0xff); // white
    for (int k = 0; k < n; k++) {
        for (int j = k+1; j < n; j++) {
            vesa->moveTo(x[k], y[k]);
            vesa->lineTo(x[j], y[j]);
        }
        if (checkforkey(27)) return;
    }
    delete[] x;
    delete[] y;
}

void drawCheckerboard(Vesa* vesa) {
    uint8_t* buffer = new uint8_t[xres];
    vesa->moveTo(0, 0);
    for (int j = 0; j < vesa->height(); j++) {
        for (int i = 0; i < vesa->width(); i++) {
            buffer[i] = i^j;
        }
        vesa->span(buffer, xres);
        vesa->moveTo(0, j);
        if (checkforkey(27)) return;
    }
    delete[] buffer;
}

int main(int argc, char** argv) {
    const int BPP = 8;
    int res[][3] = { {1600,1200,BPP}, {1280,1024,BPP}, {1024,768,BPP}, {800,600,BPP}, {640,480,BPP}, {320,200,8} };
    Vesa vesa;
    vesa.dump();
    int startMode = 0;
    if (argc > 1) {
        startMode = atoi(argv[1]);
        if (startMode > Number(res) - 1) {
            printf("Invalid mode %d\n", startMode);
            return 0;
        }
    }
    for (int i = startMode; i < Number(res); i++) {
        xres = res[i][0];
        yres = res[i][1];
        depth = res[i][2];
        if (!vesa.setMode(xres, yres, depth)) {
            printf("No mode for %dx%d@%d\n", xres, yres, 8);
        } else {
            printf("Using mode %dx%d@%d\n", xres, yres, 8);
            break;
        }
    }
    if (xres == 0 || yres == 0) {
        printf("Can't find a video mode\n");
        return(0);
    }
    makePalette(&vesa, 3, 3, 2);
    drawNgon(&vesa, 25);
    drawRectangles(&vesa, 1000);
    drawCircles(&vesa, 1000);
    drawLines(&vesa, 10000);
    drawCheckerboard(&vesa);
    return 0;
}

