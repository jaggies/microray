/*
 * matrix.c
 *
 *  Created on: Apr 26, 2019
 *      Author: jmiller
 */

#include <string.h> // memcpy()
#include <stdio.h>
#include <math.h>
#include "os.h"
#include "matrix.h"

extern float Radians(float degrees);

// Note that due to a compiler bug with cmoc, we can't use the Matrix typedef in declarations,
// so everything below takes a float[16] as a result :/

static const float identity[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

enum MatrixIndex {
    A11=0, A12=1, A13=2, A14=3,
    A21=4, A22=5, A23=6, A24=7,
    A31=8, A32=9, A33=10, A34=11,
    A41=12, A42=13, A43=14, A44=15 };

// Rotation matrix generation routines all use degrees
float* RotationMatrix(float angle, uint8_t axis, float matrix[16]) {
    const float theta = Radians(angle);
    const float SIN = sin(theta);
    const float COS = cos(theta);
    memcpy(matrix, identity, sizeof(identity));
    if (axis == X) {
        matrix[A22] = COS;  //1.0f 0.0f 0.0f 0.0f
        matrix[A23] = -SIN; //0.0f COS  SIN  0.0f
        matrix[A32] = SIN;  //0.0f -SIN COS  0.0f
        matrix[A33] = COS;  //0.0f 0.0f 0.0f 1.0f
    } else if (axis == Y) {
        matrix[A11] = COS;  //COS  0.0f -SIN 0.0f
        matrix[A13] = SIN;  //0.0f 1.0f 0.0f 0.0f
        matrix[A31] = -SIN; //SIN  0.0f COS  0.0f
        matrix[A33] = COS;  //0.0f 0.0f 0.0f 1.0f
    } else if (axis == Z) {
        matrix[A11] = COS;  // COS SIN  0.0f 0.0f
        matrix[A12] = -SIN; //-SIN COS  0.0f 0.0f
        matrix[A21] = SIN;  //0.0f 0.0f 1.0f 0.0f
        matrix[A22] = COS;  //0.0f 0.0f 0.0f 1.0f
    }
    return matrix;
}

float* RotationMatrixAxis(float angle, const float axis[3], float matrix[16])
{
    const float theta = Radians(angle);
    const float s = sin(theta);
    const float c = cos(theta);
    //const Vec3f a = normalize(axis);  // TODO
    const float x = axis[X], y = axis[Y], z = axis[Z];
    float m[16];
    m[0] = x*x*(1.0f-c)+c;
    m[1] = x*y*(1.0f-c)-z*s;
    m[2] = x*z*(1.0f-c)+y*s;
    m[4] = x*y*(1.0f-c)+z*s;
    m[5] = y*y*(1.0f-c)+c;
    m[6] = y*z*(1.0f-c)-x*s;
    m[8] = x*z*(1.0f-c)-y*s;
    m[9] = y*z*(1.0f-c)+x*s;
    m[10] = z*z*(1.0f-c)+c;
    m[3] = m[7] = m[11] = m[12] = m[13] = m[14] = 0.0f;
    m[15] = 1.0f;
    memcpy(matrix, m, sizeof(m));
    return matrix;
}

float* TranslationMatrix(float tx, float ty, float tz, float matrix[16])
{
    memcpy(matrix, identity, sizeof(identity));
    matrix[A41] = tx;
    matrix[A42] = ty;
    matrix[A43] = tz;
    return matrix;
}

float* ScaleMatrix(float sx, float sy, float sz, float matrix[16])
{
    memcpy(matrix, identity, sizeof(identity));
    matrix[A11] = sx; //sx   0.0f 0.0f 0.0f
    matrix[A22] = sy; //0.0f sy   0.0f 0.0f
    matrix[A33] = sz; //0.0f 0.0f sz   0.0f
                      //0.0f 0.0f 0.0f 1.0f
    return matrix;
}

float* TransposeMatrix(const float in[16], float out[16])
{
    float m[16]; float* p = m;
    *p++ = in[A11]; *p++ = in[A21]; *p++ = in[A31]; *p++ = in[A41];
    *p++ = in[A12]; *p++ = in[A22]; *p++ = in[A32]; *p++ = in[A42];
    *p++ = in[A13]; *p++ = in[A23]; *p++ = in[A33]; *p++ = in[A43];
    *p++ = in[A14]; *p++ = in[A24]; *p++ = in[A34], *p++ = in[A44];
    memcpy(out, m, sizeof(m));
    return out;
}

float* MultiplyMatrix(const float a[16], const float b[16], float out[16])
{
    float m[16]; float* p = m;
    *p++ = a[A11]*b[A11]+a[A12]*b[A21]+a[A13]*b[A31]+a[A14]*b[A41];
    *p++ = a[A11]*b[A12]+a[A12]*b[A22]+a[A13]*b[A32]+a[A14]*b[A42];
    *p++ = a[A11]*b[A13]+a[A12]*b[A23]+a[A13]*b[A33]+a[A14]*b[A43];
    *p++ = a[A11]*b[A14]+a[A12]*b[A24]+a[A13]*b[A34]+a[A14]*b[A44];

    *p++ = a[A21]*b[A11]+a[A22]*b[A21]+a[A23]*b[A31]+a[A24]*b[A41];
    *p++ = a[A21]*b[A12]+a[A22]*b[A22]+a[A23]*b[A32]+a[A24]*b[A42];
    *p++ = a[A21]*b[A13]+a[A22]*b[A23]+a[A23]*b[A33]+a[A24]*b[A43];
    *p++ = a[A21]*b[A14]+a[A22]*b[A24]+a[A23]*b[A34]+a[A24]*b[A44];

    *p++ = a[A31]*b[A11]+a[A32]*b[A21]+a[A33]*b[A31]+a[A34]*b[A41];
    *p++ = a[A31]*b[A12]+a[A32]*b[A22]+a[A33]*b[A32]+a[A34]*b[A42];
    *p++ = a[A31]*b[A13]+a[A32]*b[A23]+a[A33]*b[A33]+a[A34]*b[A43];
    *p++ = a[A31]*b[A14]+a[A32]*b[A24]+a[A33]*b[A34]+a[A34]*b[A44];

    *p++ = a[A41]*b[A11]+a[A42]*b[A21]+a[A43]*b[A31]+a[A44]*b[A41];
    *p++ = a[A41]*b[A12]+a[A42]*b[A22]+a[A43]*b[A32]+a[A44]*b[A42];
    *p++ = a[A41]*b[A13]+a[A42]*b[A23]+a[A43]*b[A33]+a[A44]*b[A43];
    *p++ = a[A41]*b[A14]+a[A42]*b[A24]+a[A43]*b[A34]+a[A44]*b[A44];
    memcpy(out, m, sizeof(m));
    return out;
}

float* MultiplyScalar(const float a[16], float b, float out[16]) {
    uint8_t i;
	// Ugly way to make TurboC happy...
	float *ptr = (float*) &a[0];
    for (i = 0; i < 16; i++) {
        *out++ = b * (*ptr++);
    }
    return out;
}

float* MatrixTransformVector(const float m[16], const float vin[3], float vout[3]) {
    float x = vin[X], y = vin[Y], z = vin[Z];
    vout[X] = x*m[A11] + y*m[A21] + z*m[A31] + m[A41];
    vout[Y] = x*m[A12] + y*m[A22] + z*m[A32] + m[A42];
    vout[Z] = x*m[A13] + y*m[A23] + z*m[A33] + m[A43];
    return vout;
}

float* MatrixTransformDirection(const float m[16], const float vin[3], float vout[3]) {
    float x = vin[X], y = vin[Y], z = vin[Z];
    vout[X] = x*m[A11] + y*m[A21] + z*m[A31];
    vout[Y] = x*m[A12] + y*m[A22] + z*m[A32];
    vout[Z] = x*m[A13] + y*m[A23] + z*m[A33];
    return vout;
}

float* MatrixLoadIdentity(float matrix[16]) {
    memcpy(matrix, identity, sizeof(identity));
    return matrix;
}

float* MatrixPrint(float matrix[16]) {
    printf("%f %f %f %f\n", matrix[0], matrix[1], matrix[2], matrix[3]);
    printf("%f %f %f %f\n", matrix[4], matrix[5], matrix[6], matrix[7]);
    printf("%f %f %f %f\n", matrix[8], matrix[9], matrix[10], matrix[11]);
    printf("%f %f %f %f\n", matrix[12], matrix[13], matrix[14], matrix[15]);
    return matrix;
}
