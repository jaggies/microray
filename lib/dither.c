/*
 * dither.c
 *
 * Implements various dithering techniques
 *
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "dither.h"

static const int dmat[4][4] = {
          { 0, 12,  3, 15},
          { 8,  4, 11,  7},
          { 2, 14,  1, 13},
          {10,  6,  9,  5} };

int ordered_dither(int inLevels, int outLevels, int x, int y, int grey) {
     int thresh, val, err, n;
     // the threshold for the decision
     thresh = dmat[x&3][y&3];

     // Lower of the two possible values, due to integer division
     val = grey*(outLevels-1)/inLevels;

     // Error for choosing this value
     err = grey-val*inLevels/(outLevels-1);

     // Calculate normalized value between 0 and 15 for given error
     n = 16*err*outLevels/inLevels;

    return (n > thresh) ? (val+1) : val;
 }

int diffusion_dither(int inLevels, int outLevels, int grey, int* err) {
    // Lower bound due to integer division
    int result = (grey + *err)*(outLevels-1) / inLevels;
    // Error for choosing this value
    *err += grey - result*inLevels/(outLevels-1);
    return result;
}
