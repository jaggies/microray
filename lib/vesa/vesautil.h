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
extern void makeGrayPalette(Vesa& vesa, int start, int end);

// Generates color index for use with above function
extern uint32_t compose(uint8_t r, uint8_t g, uint8_t b);

// Decomposes packed RGB format to rgb values
extern void decompose(uint32_t clr, uint8_t& r, uint8_t& g, uint8_t& b);

#endif /* LIB_VESA_VESAUTIL_H_ */
