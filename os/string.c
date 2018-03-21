#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0 // defined in ECB
size_t strlen(const char *str) {
    int n = 0;
    while (*str++ != 0)
        n++;
    return n;
}
#endif

char* strtok(char* str, const char* sep) {
	return 0; // TODO
}

char* strdup(const char *str) {
	int n = strlen(str) + 1;
	return (char*) memcpy((char*) malloc(n), str, n);
}

char* strndup(const char *str, size_t n) {
	size_t size = strlen(str);
	size = size <= n ? size : n;
	return (char*) memcpy((char*) malloc(size), str, size);
}

int sscanf(const char* s, const char* format, ...) {
	return 0;
}

int scanf(const char* format, ...) {
	return 0;
}

