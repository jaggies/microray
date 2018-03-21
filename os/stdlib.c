#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEM 16384

static char mem[MAX_MEM];
static int allocated  = 0;

FILE* stdin = 0; // TODO
FILE* stdout = 0;
FILE* stderr = 0;

void* malloc(int size) {
	if ((size+allocated) < MAX_MEM) {
		allocated += size;
		return (void*) &mem[allocated];
	}
	return 0; // no more!
}

void free(void*) {
	// TODO
}

char* getenv(const char *name) {
	return 0; // TODO
}
