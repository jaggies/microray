/*
 * testvesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef DOS
#include <conio.h> // kbhit
#endif
#include "vesa.h"
#include "vesautil.h"

static int xres = 0;
static int yres = 0;
static int depth = 8;
static const int gridX = 16;
static const int gridY = 16;

static const int resolutions[][3] = {
    {320,240,8},
    {640,480,8},
    {800,600,8},
    {1024,768,8},
    {1280,1024,8},
    {1600,1200,8} };

extern "C" {
    void sleep(int);
    int getch(void);
}

void drawColorGrid(Vesa* vesa) {
    int16_t i,j;
    vesa->color(0xff); /* white */
    for (j = 0; j < gridY; j++) {
        int16_t y1 = (long) j * (vesa->height()-1) / (gridY);
        int16_t y2 = (long) (j+1) * (vesa->height()-1) / (gridY);
        for (i = 0; i < gridX; i++) {
            int16_t x1 = (long) i * (vesa->width()-1) / (gridX);
            int16_t x2 = (long) (i+1) * (vesa->width()-1) / (gridX);
            vesa->color(j*gridX + i);
            vesa->moveTo(x1+1, y1+1);
            vesa->rectangle(x2, y2, true);
            if (kbhit()) {
                return;
            }
        }
    }
}

void drawGridlines(Vesa* vesa) {
    int16_t i;
    vesa->color(0xff); /* white */
    for (i = 0; i < gridX; i++) {
        int x = (long) i * (vesa->width()-1) / (gridX-1);
        vesa->moveTo(x, 0);
        vesa->lineTo(x, vesa->height() - 1);
    }
    for (i = 0; i < gridY; i++) {
        int y = (long) i * (vesa->height()-1) / (gridY-1);
        vesa->moveTo(0, y);
        vesa->lineTo(vesa->width()-1, y);
    }
}

void drawDiscreteRamp(Vesa* vesa, int levels) {
    long i;
    for (i = 0; i < levels; i++) {
        int x1 = i * vesa->width() / levels;
        int x2 = (i+1) * vesa->width() / levels;
        int gray = i * 255 / (levels - 1);
        vesa->color(gray);
        vesa->moveTo(x1, 0);
        vesa->rectangle(x2, vesa->height(), true);
        if (kbhit()) {
           return;
       }
    }
}

int main(int argc, char** argv) {
    int mode = 0, modeChoice=-1;
    int test = 0;
    int ch;
    const int NUMTESTS = 6;
    Vesa vesa;
    do {
        if (mode != modeChoice) {
            int xtmp = resolutions[mode][0];
            int ytmp = resolutions[mode][1];
            int dtmp = resolutions[mode][2];
            if (!vesa.setMode(xtmp, ytmp, dtmp)) {
                printf("No mode for %dx%d@%d\n", xtmp, ytmp, 8);
            } else {
                xres = xtmp;
                yres = ytmp;
                depth = dtmp;
                modeChoice = mode;
            }
        }
        vesa.color(0);
        vesa.clear();
        switch (test) {
            case 0:
                makeDitherPalette(vesa, 3, 3, 2);
                drawGridlines(&vesa);
            break;

            case 1:
                makeDitherPalette(vesa, 3, 3, 2);
                drawColorGrid(&vesa);
            break;

            case 2:
                makeDitherPalette(vesa, 3, 3, 2);
                vesa.color(0xff); // white
                vesa.clear();
            break;

            case 3:
                makeDitherPalette(vesa, 3, 3, 2);
                vesa.color(0);
                vesa.clear();
                drawDiscreteRamp(&vesa, 256);
            break;

            case 4:
                makeGrayPalette(vesa, 0, 255);
                vesa.color(0);
                vesa.clear();
                drawDiscreteRamp(&vesa, 16);
            break;

            case 5:
                makeGrayPalette(vesa, 0, 255);
                vesa.color(0);
                vesa.clear();
                drawDiscreteRamp(&vesa, 256);
            break;
        }

        switch (ch = getch()) {
            case 'r':
                mode = (++mode) % Number(resolutions);
            break;

            case ' ':
                test = (++test) % NUMTESTS;
            break;
        }
    } while (ch != 27);

    return 0;
}

