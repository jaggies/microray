/*
 * face.c
 *
 *  Created on: May 27, 2020
 *      Author: jmiller
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "world.h"

#define CHUNKSIZE (1<<2)
#define MASK (CHUNKSIZE - 1)

Face* createFace() {
    Face* result = (Face*) calloc(1, sizeof(Face));
    return result;
}

void destroyFace(Face* face) {
    free(face->vertexIndex);
    free(face->normalIndex);
    free(face->textureIndex);
    // for safety
    face->vertexIndex = face->normalIndex = face->textureIndex = NULL;
    face->nVertexIndex = face->nNormalIndex = face->nTextureIndex = 0;
    free(face);
}

void cleanup(Face* face) {
    face->vertexIndex = (size_t*) realloc(face->vertexIndex,
            face->nVertexIndex * sizeof(size_t));
    face->normalIndex = (size_t*) realloc(face->normalIndex,
            face->nNormalIndex * sizeof(size_t));
    face->textureIndex = (size_t*) realloc(face->textureIndex,
            face->nTextureIndex * sizeof(size_t));
}

void addFaceVertex(Face* face, size_t vertexIdx) {
    if (vertexIdx != -1) {
        if ((face->nVertexIndex & MASK) == 0) {
            face->vertexIndex = (size_t*) realloc(face->vertexIndex,
                    (face->nVertexIndex+CHUNKSIZE) * sizeof(size_t));
        }
        assert(face->vertexIndex);
        face->vertexIndex[face->nVertexIndex++] = vertexIdx;
    }
}

void addFaceNormal(Face* face, size_t normalIdx) {
    if (normalIdx != -1) {
        if ((face->nNormalIndex & MASK) == 0) {
            face->normalIndex = (size_t*) realloc(face->normalIndex,
                    (face->nNormalIndex+CHUNKSIZE) * sizeof(size_t));
            assert(face->normalIndex);
        }
        face->normalIndex[face->nNormalIndex++] = normalIdx;
    }
}

void addFaceTexture(Face* face, size_t textureIdx) {
    if (textureIdx != -1) {
        if ((face->nTextureIndex & MASK) == 0) {
            face->textureIndex = (size_t*) realloc(face->textureIndex,
                    (face->nTextureIndex+CHUNKSIZE) * sizeof(size_t));
            assert(face->textureIndex);
        }
        face->textureIndex[face->nTextureIndex++] = textureIdx;
    }
}

