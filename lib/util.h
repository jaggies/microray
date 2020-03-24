/*
 * util.h
 *
 *  Created on: March 28, 2018
 *      Author: jmiller
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <math.h>

#ifndef min
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#endif

#ifndef fminf
	#define fminf(a,b) min(a,b)
	#define fmaxf(a,b) max(a,b)
	#define floorf(a) floor(a)
	#define round(a) (int)((a)+0.5f)
#endif // fmin

#endif /* UTIL_H_ */
