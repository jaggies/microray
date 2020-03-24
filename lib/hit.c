/*
 * hit.c
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#include "hit.h"

void clearHit(Hit* hit) {
    hit->t = 1.0e7f;
    hit->best = 0;
    hit->inShadow = 0;
    hit->index = 1.0f; /* air */
}
