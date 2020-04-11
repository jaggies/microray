/*
 * matrix.h
 *
 *  Created on: Apr 26, 2019
 *      Author: jmiller
 */

#ifndef LIB_MATH_MATRIX_H_
#define LIB_MATH_MATRIX_H_

#include "os.h"
#include "matrix.h"

typedef float Matrix[16];

// Creates a rotation matrix.  Returns pointer to matrix.
float* RotationMatrix(float angle, uint8_t axis, float matrix[16]);

// Creates a translation matrix. Returns pointer to matrix.
float* TranslationMatrix(float tx, float ty, float tz, float matrix[16]);

// Creates a scale matrix. Returns pointer to matrix.
float* ScaleMatrix(float sx, float sy, float sz, float matrix[16]);

// Creates a rotation matrix for the given axis. Returns pointer to matrix.
float* RotationMatrixAxis(float angle, const float axis[3], float matrix[16]);

// Transposes the matrix. Returns pointer to matrix.
float* TransposeMatrix(const float in[16], float out[16]);

// Multiplies matrix a by matrix b. Returns pointer to resulting output matrix.
float* MultiplyMatrix(const float a[16], const float b[16], float out[16]);

// Multiplies matrix a by scalar b. Returns pointer to resulting output matrix.
float* MultiplyScalar(const float a[16], float b, float out[16]);

// Transforms the given vector by the matrix. Returns a pointer to the resulting matrix.
float* MatrixTransformVector(const float matrix[16], const float vin[3], float vout[3]);

// Transforms the given vector by the matrix. Returns a pointer to the resulting matrix.
float* MatrixTransformDirection(const float matrix[16], const float vin[3], float vout[3]);

// Creates an identity matrix. Returns pointer to matrix.
float* MatrixLoadIdentity(float matrix[16]);

// Prints the given matrix.
float* MatrixPrint(float matrix[16]);

#endif /* LIB_MATH_MATRIX_H_ */
