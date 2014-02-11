/*
 * shader.h
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#ifndef SHADER_H_
#define SHADER_H_

struct Shader;
struct Hit;
struct Vec3;

typedef struct ShaderOps {
    void (*evaluate)(struct Shader*, struct Hit* hit, struct Vec3* color);
} ShaderOps;

typedef struct Shader {
    ShaderOps op;
} Shader;

#endif /* SHADER_H_ */
