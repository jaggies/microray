/*
 * face.c
 *
 *  Created on: May 27, 2020
 *      Author: jmiller
 */
#include <stdlib.h>
#include <assert.h>
#include "face.h"

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

void addFaceVertex(Face* face, size_t vertexIdx) {
    if (vertexIdx != -1) {
        face->vertexIndex = (size_t*) malloc((face->nVertexIndex+1) * sizeof(size_t));
        assert(face->vertexIndex);
        face->vertexIndex[face->nVertexIndex++] = vertexIdx;
    }
}

void addFaceNormal(Face* face, size_t normalIdx) {
    if (normalIdx != -1) {
        face->normalIndex = (size_t*) malloc((face->nNormalIndex+1) * sizeof(size_t));
        assert(face->normalIndex);
        face->normalIndex[face->nNormalIndex++] = normalIdx;
    }
}

void addFaceTexture(Face* face, size_t textureIdx) {
    if (textureIdx != -1) {
        face->textureIndex = (size_t*) malloc((face->nTextureIndex+1) * sizeof(size_t));
        assert(face->textureIndex);
        face->textureIndex[face->nTextureIndex++] = textureIdx;
    }
}
