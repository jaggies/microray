/*
 * arm.c
 *
 *  Created on: April 23, 2020
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include "util.h"
#include "sphere.h"
#include "triangle.h"
#include "pointlit.h"
#include "hit.h"
#include "phongshd.h"
#include "checkshd.h"
#include "perspcam.h"
#include "world.h"
#include "raytrace.h"
#include "loader.h"
#include "netpbm.h"
#include "testload.h"
#include "dither.h"
#include "stats.h"

#define RBITS 3
#define GBITS 3
#define BBITS 2

#ifdef PROFILE
long intersections = 0;
#endif /* PROFILE */

__attribute__((weak))
void dot(int x, int y, int index) {

}

__attribute__((weak))
void pixel(uint16_t x, uint16_t y, uint8_t* rgb, void* userdata) {
	static int line = -1;

	// Default implementation does nothing
	if (line != y) {
	    printf("Line %d\n", y);
		line = y;
	}
    #ifdef USE_ERROR_DIFFUSION
	static int rerr, gerr, berr;
    int rx = diffusion_dither(1<<8, 1<<RBITS, rgb[0], &rerr);
    int gx = diffusion_dither(1<<8, 1<<GBITS, rgb[1], &gerr);
    int bx = diffusion_dither(1<<8, 1<<BBITS, rgb[2], &berr);
    #else
    uint8_t rx = ordered_dither(1<<8, 1<<RBITS, x, y, rgb[0]);
    uint8_t gx = ordered_dither(1<<8, 1<<GBITS, x, y, rgb[1]);
    uint8_t bx = ordered_dither(1<<8, 1<<BBITS, x, y, rgb[2]);
    #endif
    int index = (rx << (GBITS + BBITS)) | (gx << BBITS) | bx;
    dot(x, 1024-y, index);
}

int main(int argc, char **argv)
{
    World* world = createWorld();
    if (argc > 1) {
        printf("Loading %s\n", argv[1]);
        loadWorld(world, argv[1]);
    } else {
        printf("Loading default scene\n");
        world = testLoad(100, 100);
    }

	// Renders image, calling above pixel() routine for each pixel
    renderImage(world, pixel, NULL);
    dumpStats(stderr, world);
    destroyWorld(world);

    return 0;
}

