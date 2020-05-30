/*
 * face.h
 *
 *  Created on: May 26, 2020
 *      Author: jmiller
 */

#ifndef LIB_RAY_FACE_H_
#define LIB_RAY_FACE_H_

// A temporary structure for loading faces
typedef struct _Face {
    size_t* vertexIndex;
    size_t* normalIndex;
    size_t* textureIndex;
    size_t nVertexIndex;
    size_t nNormalIndex;
    size_t nTextureIndex;
} Face;

Face* createFace();
void destroyFace(Face*);
void cleanup(Face* face);
void addFaceVertex(Face* face, size_t vertexIdx);
void addFaceNormal(Face* face, size_t normalIdx);
void addFaceTexture(Face* face, size_t textureIdx);

#endif /* LIB_RAY_FACE_H_ */
