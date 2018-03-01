/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <assert.h>
#include "sphere.h"
#include "triangle.h"
#include "pointlight.h"
#include "hit.h"
#include "phongshader.h"
#include "checkerboardshader.h"
#include "perspectivecamera.h"
#include "world.h"
#include "raytrace.h"
#include "loader.h"
#include "netpbm.h"
#include "testload.h"

#define XRES 1024
#define YRES XRES
#define MAXDEPTH 4 // max number of reflected rays

int main(int argc, char **argv)
{
    World* world;
    NetPBM* pbm;
    const char* outpath = "out.ppm"; // TODO: get path from arguments

	if (argc > 1) {
		world = loadFile(argv[1]);
	} else {
		world = testLoad(XRES, YRES);
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
	world->width = XRES; // TODO: get from cmdline
	world->height = YRES;

	pbm = createNetPBM(outpath, world->width, world->height);

    if (!pbm) {
        printf("Failed to create output %s\n", outpath);
        return 0;
    }

    float du = 1.0f / world->width, dv = 1.0f / world->height;
    float v = 0.0f + dv * 0.5f; // emit ray from pixel centers
    for (int h = 0; h < world->height; h++, v += dv) {
        float u = 0.0f + du * 0.5f;
        for (int w = 0; w < world->width; w++, u += du) {
            Ray ray;
            Vec3 color;
            world->camera->op->makeRay(world->camera, u, 1.0f-v, &ray);
            trace(&ray, 0 /* ignore */, world, &color, MAXDEPTH);
            pbm->putPixel(pbm, w, h, &color);
        }
    }
    pbm->close(pbm);
}

