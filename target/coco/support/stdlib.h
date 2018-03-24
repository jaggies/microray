#ifndef _OS_STDLIB_H
#define _OS_STDLIB_H

#ifdef COCO
#include <cmoc.h>
extern float atof(const char*);
extern void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
extern void* malloc(int size);
extern const char* getenv(const char*);
#else
#include <stdlib.h>
#endif 

#endif // _OS_STDLIB_H
