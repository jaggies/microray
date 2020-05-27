/*
 * waveutil.h
 *
 *  Created on: May 26, 2020
 *      Author: jmiller
 */

#ifndef LIB_RAY_WAVEUTIL_H_
#define LIB_RAY_WAVEUTIL_H_

#if defined(__cplusplus)
extern "C" {
#endif

// Returns number of lines read (n > 1 for escaped lines)
extern int waveGetLine(FILE *fp, char* buffer, size_t maxLine);

// Splits line into up to maxwords arguments
extern int  split(char* str, const char** words, size_t maxwords);

// Reads a face line in the form f v1/n1/uv1 v2/n2/uv2 ...
extern void readFace(const char* str, Face* face);

// Reads a Vec2 into vec2 and returns the same
extern Vec2* readvec2(const char *str, Vec2* vec2);

// Reads a Vec3 into vec3 and returns the same
extern Vec3* readvec3(const char *str, Vec3* vec3);

// Caller is responsible for deallocation.
char* fullpath(const char* path, const char* name);

#if defined(__cplusplus)
}
#endif

#endif /* LIB_RAY_WAVEUTIL_H_ */
