#ifndef _OS_STDIO_H
#define _OS_STDIO_H

#ifdef COCO

typedef struct _FILE {
   int fd;
   char buff[256];
} FILE;
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

extern FILE * fopen(const char* filename, const char* mode);
extern int feof(FILE *fp);
extern char * fgets(char* str, int size, FILE * stream);
extern int fclose(FILE *stream);
extern int fputc(int c, FILE *stream);
extern int fprintf(FILE* stream, const char* format, ...);
#else
#include <stdlib.h>
#endif 

#endif // _OS_STDIO_H
