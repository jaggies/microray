/*
 * testvesa.cpp
 *
 *  Created on: Mar 28, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include "vesa.h"

int main(int argc, char** argv) {
    Vesa vesa;
    vesa.dump();
    printf("Try 640x480 8bpp\n");
    vesa.setMode(640, 480, 8);
    int i;
    for (i = 0; i < 480; i++) {
        vesa.dot(i, i, i);
    }
//    printf("Try 640x480 15bpp\n");
//    vesa.setMode(640, 480, 15);
//    printf("Try 640x480 16bpp\n");
//    vesa.setMode(640, 480, 16);
//    printf("Try 720x480 15bpp\n");
//    vesa.setMode(720, 480, 15);
//    printf("Try 1280x1024 8bpp\n");
//    vesa.setMode(1280, 1024, 8);
//    printf("Try 1600x1200 8bpp\n");
//    vesa.setMode(1600, 1200, 8);
//    vesa.dot(0,0,0);
}

