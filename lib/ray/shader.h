/*
 * shader.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef SHADER_H_
#define SHADER_H_

typedef struct ShaderOps ShaderOps;

struct Shader {
    ShaderOps* op; /* these must be first in all "inherited" classes */
};

typedef struct Shader Shader;

struct ShaderOps {
    void (*evaluate)(Shader*, Hit* hit, Vec3* color);
    float (*getReflectionAmount)(Shader* shader);
    float (*getTransmissionAmount)(Shader* shader);
    float (*getIndexOfRefraction)(Shader* shader);
};

#endif /* SHADER_H_ */
