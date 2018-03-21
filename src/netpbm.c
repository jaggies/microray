/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h> /* malloc */
#include <math.h>
#include "vec3.h"
#include "netpbm.h"

static int min(int a, int b) {
    return a < b ? a : b;
}

static int max(int a, int b) {
    return a > b ? a : b;
}

static
void closeNetPBM(NetPBM* pbm) {
    if (pbm->fp) {
        fclose(pbm->fp);
        pbm->fp = 0;
    }
}

static
void putPixel(NetPBM* pbm, int x, int y, Vec3* color) {
    int r = min(255, max(0, (int)round(color->x * 255)));
    int g = min(255, max(0, (int)round(color->y * 255)));
    int b = min(255, max(0, (int)round(color->z * 255)));
    fputc(r, pbm->fp);
    fputc(g, pbm->fp);
    fputc(b, pbm->fp);
}

NetPBM* createNetPBM(const char* path, int width, int height) {
    NetPBM* pbm;
    FILE* fp = fopen(path, "w");
    if (!fp) {
        return 0;
    }
    pbm = (NetPBM*) malloc(sizeof(NetPBM));
    pbm->putPixel = putPixel;
    pbm->close = closeNetPBM;
    pbm->fp = fp;
    fprintf(fp, "P6 %d %d 255\n", width, height);
    return pbm;
}

