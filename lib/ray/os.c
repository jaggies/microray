/*
 * os.c
 *
 *  Created on: May 25, 2020
 *      Author: jmiller
 */

#include "os.h"

const char* basename(const char *path) {
    const char* ptr = path + strlen(path);
    while (ptr >= path && !(*ptr == '/' || *ptr == '\\')) {
        ptr--;
    }
    ptr++; // first character past '/' or starting letter if not found
    return ptr;
}
