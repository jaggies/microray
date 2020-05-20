/*
 * netpbm.h
 *
 *  Created on: March 1, 2018
 *      Author: jmiller
 */

#ifndef NETPBM_H_
#define NETPBM_H_

#include "os.h"

typedef bool (*PixelCallback)(uint16_t x, uint16_t y, uint8_t* rgb, void* userData);
enum { NETPBM_NONE = 0, NETPBM_READ, NETPBM_WRITE };
typedef struct NetPBM NetPBM;

struct NetPBM {
    int (*open)(NetPBM* pbm, const char* path, int* width, int* height, int* depth, int mode);
    bool (*read)(NetPBM* pbm, PixelCallback cb, void* clientData);
    void (*close)(NetPBM* pbm);
    void (*write)(NetPBM* pbm, const unsigned char color[3]);
    void (*reset)(NetPBM* pbm);
    int mode;
    int width;
    int height;
    int depth;
    FILE* fp;
};

#ifdef __cplusplus
extern "C" {
#endif

NetPBM* createNetPBM();
void destroyPBM(NetPBM*);

#ifdef __cplusplus
}
#endif

#endif /* NETPBM_H_ */
