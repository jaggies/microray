#ifndef OS_H
#define OS_H

#define Number(a) (sizeof(a) / sizeof(a[0]))

#ifdef DOS
typedef char int8_t;
typedef short int16_t;
typedef long long int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long long uint32_t;
#define nullptr ((void*) 0)
#define bzero(ptr, size) memset(ptr, 0, size)
#else
#include <stdint.h>
#endif

#endif // OS_H
