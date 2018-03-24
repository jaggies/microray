#ifndef _OS_MATH_H
#define _OS_MATH_H

#ifdef COCO

#define M_PI (3.14159265358979323846f)

#define fmin(x,y) fminf(x,y)
#define fmax(x,y) fmaxf(x,y)
#define sqrt(x) sqrtf(x)
#define floor(x) floorf(x)
#define tan(x) tanf(x)
#define fabs(x) fabsf(x)
#define pow(x, y) powf(x,y)
#define round(x) roundf(x)

extern float fminf(float x, float y);
extern float fmaxf(float x, float y);
extern float sqrtf(float x);
extern float floorf(float x);
extern float tanf(float x);
extern float fabsf(float x);
extern float pow(float x, float y);
extern float round(float x);
extern float powf(float x, float y);
#else 
#include <math.h>
#endif

#endif // _OS_MATH_H
