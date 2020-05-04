/*
 * vesautil.cpp
 *
 *  Created on: Apr 11, 2020
 *      Author: jmiller
 */

#include <stdlib.h>
#include "vesa.h"
#include "vesautil.h"

int checkforkey(int key) {
#ifdef DOS
    if (kbhit()) {
        if (key == 0)
            return true;
        else if (key == getch())
            return true;
    }
#endif
    return false;
}

void makeDitherPalette(Vesa& vesa, int rbits, int gbits, int bbits) {
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

void makeGrayPalette(Vesa& vesa, int start, int end, int mask) {
    for (int i = start; i <= end; i++) {
        uint8_t color = start+i;
        vesa.palette(i-start,
                (mask & 4) ? color : 0,
                (mask & 2) ? color : 0,
                (mask & 1) ? color : 0);
    }
}

uint32_t compose(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 16) | (g << 8) | b;
}

void decompose(uint32_t clr, uint8_t& r, uint8_t& g, uint8_t& b) {
    r = (clr >> 16) & 0xff;
    g = (clr >> 8) & 0xff;
    b = clr & 0xff;
}

