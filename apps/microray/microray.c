/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* free */
#include <assert.h>
#include "os.h"
#include "util.h"
#include "hit.h"
#include "shader.h"
#include "sphere.h"
#include "triangle.h"
#include "pointlit.h"
#include "phongshd.h"
#include "checkshd.h"
#include "perspcam.h"
#include "world.h"
#include "raytrace.h"
#include "loader.h"
#include "netpbm.h"
#include "testload.h"
#include "stats.h"

static bool pixel(uint16_t x, uint16_t y, uint8_t* rgb, void* userdata) {
    NetPBM* pbm = (NetPBM*) userdata;
    pbm->write(pbm, rgb);
    if (x == 0) {
        fprintf(stdout, "Line %d\r", y);
		fflush(stdout);
    }
    return true;
}

static void renderToFile(World* world, const char* outpath)
{
    NetPBM* pbm = createNetPBM(outpath);
    if (pbm->open(pbm, outpath, &world->width, &world->height, &world->depth, NETPBM_WRITE)) {
        printf("Rendering scene (%dx%d)\n", world->width, world->height);
        renderImage(world, pixel, pbm);
        pbm->close(pbm);
    } else {
        printf("Can't write image '%s'\n", outpath);
    }
    destroyPBM(pbm);
}

int main(int argc, char **argv)
{
    World* world;

    printf("*** MICRORAY ***\n");

    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 0;
    }

    if (loadWorld(world = createWorld(), argv[1])) {
        char* outpath = getImagePath(argv[1]);

        renderToFile(world, outpath);
        dumpStats(stderr, world);

        free(outpath);
    } else {
        printf("Failed to load file '%s'\n", argv[1]);
    }

    destroyWorld(world);

    return 0;
}

