/*
 * netpbm.h
 *
 *  Created on: March 1, 2018
 *      Author: jmiller
 */

#ifndef NETPBM_H_
#define NETPBM_H_

typedef void (*PixelCallback)(void* clientData, int x, int y, unsigned char pixel[3]);
enum { NETPBM_NONE = 0, NETPBM_READ, NETPBM_WRITE };
typedef struct NetPBM NetPBM;

struct NetPBM {
    int (*open)(NetPBM* pbm, const char* path, int* width, int* height, int* depth, int mode);
    void (*read)(NetPBM* pbm, PixelCallback cb, void* clientData);
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

#ifdef __cplusplus
}
#endif

#endif /* NETPBM_H_ */
