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
        vesa->circle(rand()%512, true);
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
    uint16_t * x = (uint16_t*) malloc(n*sizeof(uint16_t));
    uint16_t * y = (uint16_t*) malloc(n*sizeof(uint16_t));

    for (int i = 0; i < n; i++) {
        float alpha = 2.0f*M_PI*i / (n-1);
        x[i] = cos(alpha) * 512.0f + xres/2;
        y[i] = sin(alpha) * 512.0f + yres/2;
    }
    vesa->color(0xff); // white
    for (int k = 0; k < n; k++) {
        for (int j = k+1; j < n; j++) {
            vesa->moveTo(x[k], y[k]);
            vesa->lineTo(x[j], y[j]);
        }
    }
    free(x);
    free(y);
}

void drawCheckerboard(Vesa* vesa) {
    for (int j = 0; j < vesa->height(); j++) {
        for (int i = 0; i < vesa->width(); i++) {
            vesa->moveTo(i, j);
            vesa->color(i^j);
            vesa->dot();
        }
    }
}

int main(int argc, char** argv) {
    Vesa vesa;
    vesa.dump();
    vesa.setMode(xres, yres, 8);
    makePalette(&vesa, 3, 3, 2);
    drawNgon(&vesa, 20);
    drawRectangles(&vesa, 100);
    drawCircles(&vesa, 100);
    drawLines(&vesa, 10000);
    drawCheckerboard(&vesa);
}

