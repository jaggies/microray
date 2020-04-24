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

#ifdef PROFILE
long intersections = 0;
#endif /* PROFILE */

__attribute__((weak))
void pixel(uint16_t x, uint16_t y, uint8_t* rgb, void* userdata) {
	static int line = -1;
	// Default implementation does nothing
	if (line != y) {
	    printf("Line %d\n", y);
		line = y;
	}
}

int main(int argc, char **argv)
{
    World* world;
    if (argc > 1) {
        printf("Loading %s\n", argv[1]);
        world = loadFile(argv[1]);
    } else {
        printf("Loading default scene\n");
        world = testLoad(100, 100);
    }
    if (world->nShapes == 0) {
        printf("World contains no shapes, exiting\n");
        return 0;
    }
    if (world->nLights == 0) {
        printf("World contains no lights, exiting\n");
        return 0;
    }
    if (!world->camera) {
        printf("World contains no camera, exiting\n");
        return 0;
    }

	// Renders image, calling above pixel() routine for each pixel
    renderImage(world, pixel, NULL);

#ifdef PROFILE
    printf("%ld intersections\n", intersections);
#endif /* PROFILE */
    return 0;
}

