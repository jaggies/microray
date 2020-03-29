/*
 * testvesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include "vesa.h"

static void makePalette(Vesa& vesa, int rbits, int gbits, int bbits) {
    int index = 0;
    for (index = 0; index < 256; index++) {
        int r = (index >> (gbits + bbits)) << (8-rbits);
        int g = (index >> (bbits)) << (8-gbits);
        int b = (index) << (8-bbits);
        vesa.palette(index, r, g, b);
    }
}

int main(int argc, char** argv) {
    Vesa vesa;
    vesa.dump();
    vesa.setMode(1280, 1024, 8);
    for (int j = 0; j < vesa.height(); j++) {
        for (int i = 0; i < vesa.width(); i++) {
            vesa.dot(i, j, i^j);
        }
    }
    makePalette(vesa, 3, 3, 2);
}

