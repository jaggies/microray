/*
 * netpbm.h
 *
 *  Created on: March 1, 2018
 *      Author: jmiller
 */

#ifndef NETPBM_H_
#define NETPBM_H_

typedef struct _NetPBM NetPBM;
typedef struct _NetPBM {
    void (*putPixel)(NetPBM* pbm, int x, int y, Vec3* color);
	void (*close)(NetPBM* pbm);
	int width;
	int height;
	FILE* fp;
} NetPBM;

extern NetPBM* createNetPBM(const char* path, int width, int height);

#endif /* NETPBM_H_ */
