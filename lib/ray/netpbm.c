/*
 * main.c
 *
 *  Created on: Feb 9, 2014
 *      Author: jmiller
 */

#include <stdio.h>
#include <stdlib.h> /* malloc() */
#include <string.h> /* memset() */
#include "netpbm.h"

static int openNetPBM(NetPBM* pbm, const char* path, int* width, int* height, int* depth, int mode)
{
    if (pbm->fp) {
        fclose(pbm->fp);
        pbm->fp = 0;
    }
    if (mode == NETPBM_WRITE) {
        if (*width == 0 || *height == 0) {
            printf("Width and height must be non-zero\n");
            return 0;
        }
        pbm->fp = fopen(path, "wb");
        if (!pbm->fp) {
            printf("Couldn't open file %s for write!\n", path);
            return 0;
        }
        fprintf(pbm->fp, "P6 %d %d %d\n", *width, *height, *depth);
        return 1;
    } else if (mode == NETPBM_READ) {
        char hdr[8];
        int n;
        pbm->fp = fopen(path, "rb");
        if (!pbm->fp) {
            printf("Couldn't open pbm file '%s'\n", path);
            return 0;
        }
        n = fscanf(pbm->fp, "%s %d %d %d\n", hdr, &pbm->width, &pbm->height, &pbm->depth);
        if (n != 4 || pbm->width == 0 || pbm->height == 0) {
            printf("Invalid width/height while reading pbm header\n");
            fclose(pbm->fp);
            pbm->fp = 0;
            return 0;
        }
        *width = pbm->width;
        *height = pbm->height;
        *depth = pbm->depth; /* TODO: convert to # bits */
        return 1;
    } else {
        printf("Invalid mode: %d\n", mode);
    }
    return 0;
}

static void readNetPBM(NetPBM* pbm, PixelCallback cb, void* clientData)
{
    int x, y;
    unsigned char pixel[3];
    if (!pbm->fp) {
        printf("Call open() first\n");
        return;
    }
    for (y = 0; y < pbm->height; y++) {
        for (x = 0; x < pbm->width; x++) {
            pixel[0] = fgetc(pbm->fp);
            pixel[1] = fgetc(pbm->fp);
            pixel[2] = fgetc(pbm->fp);
            (*cb)(clientData, x, y, pixel);
            if (feof(pbm->fp)) {
                printf("EOF detected\n");
                return;
            }
        }
    }
}

static void closeNetPBM(NetPBM* pbm) {
    if (pbm->fp) {
        fclose(pbm->fp);
        pbm->fp = 0;
        pbm->width = pbm->height = pbm->depth = 0;
    }
}

static void writeNetPBM(NetPBM* pbm, unsigned char color[3]) {
    fputc(color[0], pbm->fp);
    fputc(color[1], pbm->fp);
    fputc(color[2], pbm->fp);
}

NetPBM* createNetPBM(const char* path) {
    NetPBM* pbm;
    pbm = (NetPBM*) malloc(sizeof(NetPBM));
    memset(pbm, 0, sizeof(NetPBM));
    pbm->open = openNetPBM;
    pbm->read = readNetPBM;
    pbm->write = writeNetPBM;
    pbm->close = closeNetPBM;
    return pbm;
}

