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

int ordered_dither(int inLevels, int outLevels, int x, int y, int grey);
int diffusion_dither(int inLevels, int outLevels, int grey, int* err);

#endif // DITHER_H
