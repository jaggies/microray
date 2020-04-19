/*
 * dither.h
 *
 * Various dithering techniques
 *
 *      Author: jmiller
 */

#ifndef DITHER_H
#define DITHER_H

#include "util.h"

class Dither {
    public:
        Dither(int inLevels, int outLevels);
        int ordered(int x, int y, int gray);
        int diffusion(int grey, int* err);
    private:
        int mInLevels;
        int mOutLevels;
};

#endif // DITHER_H
