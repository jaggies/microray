/*
 * shader.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef SHADER_H_
#define SHADER_H_

typedef struct Hit Hit;
typedef struct Vec3 Vec3;
typedef struct Shader Shader;
typedef struct ShaderOps ShaderOps;

struct Shader {
    ShaderOps* op; /* these must be first in all "inherited" classes */
};

struct ShaderOps {
    void (*evaluate)(Shader*, Hit* hit, Vec3* color);
    float (*getReflectionAmount)(Shader* shader);
    float (*getTransmissionAmount)(Shader* shader);
    float (*getIndexOfRefraction)(Shader* shader);
};

#endif /* SHADER_H_ */
