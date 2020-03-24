#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifdef AMIGAOS

#define HEAPSIZE (1024*1024)
#define STACKSIZE (64*1024)

size_t __stack=65536; /* 64KB stack-size */
char __heap[HEAPSIZE],*__heapptr=__heap;
size_t __heapsize = HEAPSIZE;

float fminf(float a, float b) { 
	return a < b ? a : b;
}

float fmaxf(float a, float b) {
	return a > b ? a : b;
}

float floorf(float a) {
	return (float)(int) a;
}

char* strdup(const char *str) {
    int n = strlen(str) + 1;
    return (char*) memcpy((char*) malloc(n), str, n);
}

#endif
