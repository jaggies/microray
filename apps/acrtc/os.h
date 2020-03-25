#ifndef OS_H
#define OS_H

#define Number(a) (sizeof(a) / sizeof(a[0]))

#ifdef DOS
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef char int8_t;
typedef short int16_t;
#else
#include <stdint.h>
#endif  // DOS

#endif // OS_H
