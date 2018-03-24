#ifndef _OS_STRING_H
#define _OS_STRING_H

#ifdef COCO
#include <cmoc.h>
extern char * strtok(char * str, const char * sep);
extern char * strdup(const char *s1); 
extern char * strndup(const char *s1, size_t n);
extern int sscanf(const char* s, const char* format, ...);
extern int scanf(const char* format, ...);
#else
#include <string.h>
#endif

#endif // _OS_STRING_H

