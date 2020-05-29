/*
 * os.c
 *
 *  Created on: May 25, 2020
 *      Author: jmiller
 */

#include <math.h>
#include "os.h"

static const char* slash = "/"; // TODO: DOS equivalent
#define SLASH (*slash)

#if !defined(basename)
const char* basename(const char *path) {
    const char* ptr = path + strlen(path);
    while (ptr >= path && !(*ptr == SLASH)) {
        ptr--;
    }
    ptr++; // first character past '/' or starting letter if not found
    return ptr;
}
#endif

#if !defined(strcasecmp)
int strcasecmp(const char *s1, const char *s2) {
    int result;
    unsigned const char *p1, *p2;
    if (s1 == s2) return 0;
    p1 = (const unsigned char*) s1;
    p2 = (const unsigned char*) s2;
    while ( (result = (tolower(*p1) - tolower(*p2++))) == 0) {
        if (*p1++ == 0) {
            break;
        }
    }
    return result;
}
#endif

#if !defined(modf)
float fmodf(float x, float y) {
    // This isn't as good as true modf, but accurate to 5 digits around +-10
    return x - y*(int)(x/y);
}
#endif

#if !defined(_dirname)
char* dirname(char* path) {
    static char* dot = ".";
    char *ptr;

    // Ugly, pre-defined behavior for null or empty string.
    if (!path || *path == 0) return dot;

    // Check for all slashes
    ptr = path;
    while (*ptr && *ptr == SLASH ) {
        ptr++;
    }

    // Reaching the end means all slashes. Return "//" or "/"
    if (*ptr == 0) return "/";

    // Remove all trailing slash chars
    ptr = path + strlen(path) - 1;
    while (ptr > path && *ptr == SLASH) {
        *ptr-- = 0;
    }

    // Now remove all non-slash characters
    ptr = path + strlen(path) - 1;
    while (ptr >= path && *ptr != SLASH) {
        *ptr-- = 0;
    }

    ptr = path + strlen(path) - 1;
    while (ptr > path && *ptr == SLASH) {
        *ptr-- = 0;
    }

    if (ptr == path && *ptr != SLASH || *ptr == 0) return dot;

    return path;
}
#endif

#if !defined(_isblank)
int isblank(char t) {
    return t == '\t' || t == ' ';
}
#endif
