/*
 * waveutil.c
 *
 *  Created on: May 26, 2020
 *      Author: jmiller
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // malloc
#include <ctype.h> // isdigit
#include <assert.h>
#include "os.h"
#include "vec2.h"
#include "vec3.h"
#include "face.h"
#include "waveutil.h"

enum Item { VEC=0, UV, NORM };
enum LineState { UNKNOWN, DIGIT, WHITESPACE, SLASH };

int waveGetLine(FILE *fp, char* buffer, size_t maxLine)
{
    if (fgets(buffer, maxLine, fp)) {
        // Join lines containing '\'.
        char* join = strrchr(buffer, '\\');
        if (join) {
            size_t remaining = maxLine - (join - buffer);
            return 1 + waveGetLine(fp, join, remaining);
        }
    }
    return 1;
}

int split(char* str, const char** words, size_t maxwords)
{
    int n = 0;
    while (n < maxwords && (words[n] = strtok(n == 0 ? str : NULL, " \t\n")))
        n++;
    assert(n < maxwords);
    return n;
}

void readFace(const char* str, Face* face)
{
    int item = VEC;
    int value = 0;
    const char *ptr = str;
    int state, laststate;
    size_t i = 0;
    const int N = strlen(str);
    char ch = str[i];

    state = laststate = UNKNOWN;

    // Skip forward to first digit
    while (!isdigit(ch) && ch != '/' && i < N) {
        ch = str[++i];
    }
    if (i == N) {
        fprintf(stderr, "Internal ERROR: No digit in face string '%s'\n", str);
        return;
    }
    ptr = str + i;

    do {
        if (isdigit(*ptr)) {
            value = value*10 + (*ptr - '0');
            state = DIGIT;
        } else if (*ptr == '/') {
            state = SLASH;
        } else if (isblank(*ptr) || *ptr == '\n' || *ptr == '\r') {
            state = WHITESPACE;
        } else {
            fprintf(stderr, "readTriplets(): unknown character '%c'\n", *ptr);
            state = UNKNOWN;
        }
        if (laststate == DIGIT && state != DIGIT) {
            // Use value-1 since vertices start with 1 for wavefront objects
            if (item == VEC) {
                addFaceVertex(face, value - 1);
            } else if (item == UV) {
                addFaceTexture(face, value - 1);
            } else if (item == NORM) {
                addFaceNormal(face, value - 1);
            }
            value = 0; // Prepare for next value
        }
        if (state == SLASH) {
            item = (item+1) % 3;
        }
        else if (state == WHITESPACE) {
            item = VEC;
        }
        laststate = state;
    } while (*++ptr != '\0');
}

Vec2* readvec2(const char *str, Vec2* vec2)
{
    if (sscanf(str, "%f %f", &vec2->x, &vec2->y) != 2) {
        fprintf(stderr, "Short read of Vec2\n");
        vec2->x = vec2->y = 0.0f;
    }
    return vec2;
}

Vec3* readvec3(const char *str, Vec3* vec3)
{
    if (sscanf(str, "%f %f %f", &vec3->x, &vec3->y, &vec3->z) != 3) {
        fprintf(stderr, "Short read of Vec2\n");
        vec3->x = vec3->y = vec3->z = 0.0f;
    }
    return vec3;
}

char* fullpath(const char* path, const char* name)
{
    // Caller is responsible for deallocation.
    char* str = (char*)malloc(strlen(path) + strlen(name) + 2);
    sprintf(str, "%s/%s", path, name);
    return str;
}




