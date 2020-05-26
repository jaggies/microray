#ifndef OS_H
#define OS_H

#define Number(a) (sizeof(a) / sizeof(a[0]))

enum { X = 0, Y = 1, Z = 2 };

#ifdef DOS
#include <conio.h> // kbhit(), getch()
typedef char int8_t;
typedef short int16_t;
typedef long long int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long long uint32_t;
#define nullptr ((void*) 0)
#define bzero(ptr, size) memset(ptr, 0, size)
#if !defined(bool) // Couldn't get this to work in both TC & Linux. DOS it doesn't exist; Linux only C++
typedef int bool;
#endif

#if defined(__cplusplus)
extern "C" {
#endif

extern const char* basename(const char *path);

#if defined(__cplusplus)
}
#endif

#else
#include <stdint.h>
#include <stddef.h> // size_t
#include <linux/limits.h> // PATH_MAX
#include <libgen.h> // basename()
#endif

#if !defined(__cplusplus)
#if !defined(bool)
typedef int bool;
#endif
#endif

#if !defined(false)
#define false 0
#endif

#if !defined(true)
#define true 1
#endif

#endif // OS_H
