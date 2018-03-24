#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXP_A 184
#define EXP_C 16249 

union {
	float d;
	struct {
		#ifdef LITTLE_ENDIAN
		short j, i;
		#else
		short i, j;
		#endif
	} n;
} eco;

static float expf(float y) {
	eco.n.i = (int) (EXP_A*(y) + (EXP_C));
	eco.n.j = 0;
	return eco.d;
}

float fminf(float x, float y) {
    return x < y ? x : y;
}

float fmaxf(float x, float y) {
    return x > y ? x : y;
}

float logf(float y) {
	int * nTemp = (int*)&y;
	y = (*nTemp) >> 16;
	return (y - EXP_C) / EXP_A;
}

float powf(float b, float p) {
	return expf(logf(b) * p);
}

float floorf(float x) {
	return (float)(int)x; // TODO
}

float tanf(float x) {
	return 0.0f; // TODO
}

float fabsf(float x) {
	return x < 0.0f ? -x : x;
}

float roundf(float x) {
	return (float) (int) x;
}

