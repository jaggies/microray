/*
 * waveload.h
 *
 *  Created on: May 19, 2020
 *      Author: jmiller
 */

#ifndef LIB_RAY_WAVELOAD_H_
#define LIB_RAY_WAVELOAD_H_

#define WAVE_MAX_LINE 256 // maximum length of a line
#define MAX_ARGS 10 // maximum number of arguments on a line

extern bool loadWavefront(World* world, const char *filename, const char *options);

#endif /* LIB_RAY_WAVELOAD_H_ */
