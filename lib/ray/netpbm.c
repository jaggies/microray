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
            fprintf(stderr, "Width and height must be non-zero\n");
            return 0;
        }
        pbm->fp = fopen(path, "wb");
        if (!pbm->fp) {
            fprintf(stderr, "Couldn't open file %s for write!\n", path);
            return 0;
        }
        pbm->width = *width;
        pbm->height = *height;
        pbm->depth = *depth;
        pbm->mode = mode;
        // TODO: Actually honor mode!
        fprintf(pbm->fp, "P6 %d %d %d\n", *width, *height, *depth);
        return 1;
    } else if (mode == NETPBM_READ) {
        int n;
        pbm->fp = fopen(path, "rb");
        if (!pbm->fp) {
            fprintf(stderr, "Couldn't open pbm file '%s'\n", path);
            return 0;
        }
        n = fscanf(pbm->fp, "P%d", &pbm->mode);
        if (n == 1) switch(pbm->mode) {
            case 4:
                n = fscanf(pbm->fp, "%d %d\n", &pbm->width, &pbm->height);
                pbm->depth = 1;
            break;
            case 5:
            case 6:
                n = fscanf(pbm->fp, "%d %d %d\n", &pbm->width, &pbm->height, &pbm->depth);
                if (n < 3) {
                    fprintf(stderr, "Invalid file\n");
                    fclose(pbm->fp);
                    pbm->fp = 0;
                    return 0;
                }
            break;
            default:
                fprintf(stderr, "Invalid mode %d\n", pbm->mode);
                fclose(pbm->fp);
                pbm->fp = 0;
                return 0;
            break;
        }
        if (pbm->width == 0 || pbm->height == 0) {
            fprintf(stderr, "Invalid width/height while reading pbm header\n");
            fclose(pbm->fp);
            pbm->fp = 0;
            return 0;
        }
        *width = pbm->width;
        *height = pbm->height;
        *depth = pbm->depth; /* TODO: convert to # bits */
        return 1;
    } else {
        fprintf(stderr, "Invalid mode: %d\n", mode);
    }
    return 0;
}

static bool readNetPBM(NetPBM* pbm, PixelCallback cb, void* clientData)
{
    uint16_t x, y;
    unsigned char pixel[3];
    if (!pbm->fp) {
        printf("Call open() first\n");
        return false;
    }
    for (y = 0; y < pbm->height; y++) {
        unsigned char pix = 0; // for bitmap mode
        for (x = 0; x < pbm->width; x++) {
            if (pbm->mode == 4) {
                // Bitmap
                if (!(x%8)) {
                    pix = fgetc(pbm->fp);
                }
                pixel[0] =  pixel[1] = pixel[2] = (pix & 0x80) ? 0 : 255;
                pix <<= 1;
            } else if (pbm->mode == 5) {
                // Grayscale
                pixel[0] =  pixel[1] = pixel[2] = fgetc(pbm->fp);
            } else if (pbm->mode == 6) {
                // RGB
                pixel[0] = fgetc(pbm->fp);
                pixel[1] = fgetc(pbm->fp);
                pixel[2] = fgetc(pbm->fp);
            }
            if (!(*cb)(x, y, pixel, clientData)) {
                return false; // client error
            }
            if (feof(pbm->fp)) {
                printf("EOF detected\n");
                return false;
            }
        }
    }
    return true;
}

static void closeNetPBM(NetPBM* pbm) {
    if (pbm->fp) {
        fclose(pbm->fp);
        pbm->fp = 0;
        pbm->width = pbm->height = pbm->depth = 0;
    }
}

static void writeNetPBM(NetPBM* pbm, const unsigned char color[3]) {
    fputc(color[0], pbm->fp);
    fputc(color[1], pbm->fp);
    fputc(color[2], pbm->fp);
}

static void resetNetPBM(NetPBM* pbm) {
    fseek(pbm->fp, 0L, SEEK_SET);
}

NetPBM* createNetPBM() {
    NetPBM* pbm;
    pbm = (NetPBM*) malloc(sizeof(NetPBM));
    memset(pbm, 0, sizeof(NetPBM));
    pbm->open = openNetPBM;
    pbm->read = readNetPBM;
    pbm->write = writeNetPBM;
    pbm->close = closeNetPBM;
    pbm->reset = resetNetPBM;
    return pbm;
}

