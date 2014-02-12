/*
 * hit.c
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#include "hit.h"

void clearHit(Hit* hit) {
    hit->t = 1.0e7;
    hit->best = 0;
    hit->inShadow = 0;
}
