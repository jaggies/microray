/*
 * testvesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vesa.h"

static const int xres = 1280;
static const int yres = 1024;

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
    }
    delete[] buffer;
}

int main(int argc, char** argv) {
    Vesa vesa;
    vesa.dump();
    if (!vesa.setMode(xres, yres, 8)) {
        printf("Can't find mode for %dx%d@%d", xres, yres, 8);
        return 0;
    }
    makePalette(&vesa, 3, 3, 2);
    drawNgon(&vesa, 25);
    drawRectangles(&vesa, 1000);
    drawCircles(&vesa, 1000);
    drawLines(&vesa, 1000);
    drawCheckerboard(&vesa);
    return 0;
}

