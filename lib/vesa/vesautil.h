/*
 * vesautil.h
 *
 *  Created on: Apr 11, 2020
 *      Author: jmiller
 */

#ifndef LIB_VESA_VESAUTIL_H_
#define LIB_VESA_VESAUTIL_H_

#include "vesa.h"

// This creates a palette compatible with dithering code for given number of bits.
extern void makeDitherPalette(Vesa& vesa, int rbits, int gbits, int bbits);

// This creates a grayscale palette starting at entry 0 = (start, start, start)
// Mask is a bitfield indicating which channels to use (RGB = 4, 2, 1)
extern void makeGrayPalette(Vesa& vesa, int start, int end, int mask = 7);

// Generates color index for use with above function
extern uint32_t compose(uint8_t r, uint8_t g, uint8_t b);

// Decomposes packed RGB format to rgb values
extern void decompose(uint32_t clr, uint8_t& r, uint8_t& g, uint8_t& b);

// Check for the given key. Returns true if found. Passing 0 for key means any key counts.
extern int checkforkey(int key);

#endif /* LIB_VESA_VESAUTIL_H_ */
