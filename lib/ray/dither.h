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

extern int ordered_dither(int inLevels, int outLevels, int x, int y, int grey);
extern int diffusion_dither(int inLevels, int outLevels, int grey, int* err);
extern int dither(int rbits, int gbits, int bbits, int x, int y, uint8_t r, uint8_t g, uint8_t b);

#endif // DITHER_H
