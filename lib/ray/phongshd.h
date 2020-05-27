/*
 * phoneshader.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef PHONESHADER_H_
#define PHONESHADER_H_

#include "shader.h"
#include "vec3.h"

typedef struct PhongShader PhongShader;

struct PhongShader {
    ShaderOps* op;
    Vec3 diffuse;
    Vec3 specular;
    Vec3 ambient;
    float exponent; /* specular exponent */
    float index; /* index of refraction */
    float reflect; /* amount of reflection */
    float transmit; /* amount of transmission */
};

extern PhongShader* createPhongShader(Vec3* diffuse, Vec3* specular, Vec3* ambient, float exponent,
        float index, float reflect, float transmit);

#endif /* PHONESHADER_H_ */
