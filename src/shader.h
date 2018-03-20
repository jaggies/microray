/*
 * shader.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef SHADER_H_
#define SHADER_H_

typedef struct _Hit Hit;
typedef struct _Vec3 Vec3;
typedef struct _ShaderOps ShaderOps;

typedef struct _Shader {
    ShaderOps* op; /* these must be first in all "inherited" classes */
} Shader;

typedef struct _ShaderOps {
    void (*evaluate)(Shader*, Hit* hit, Vec3* color);
    float (*getReflectionAmount)(Shader* shader);
    float (*getTransmissionAmount)(Shader* shader);
    float (*getIndexOfRefraction)(Shader* shader);
} ShaderOps;

#endif /* SHADER_H_ */
