/*
 * checkerboard.h
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#ifndef CHECKERBOARD_H_
#define CHECKERBOARD_H_

#include "shader.h"
#include "vec2.h"
#include "vec3.h"

typedef struct CheckerboardShader {
    ShaderOps* op;
    Vec3 color1;
    Vec3 color2;
    Vec2 scale;
    Vec2 bias;
    Shader* target;
    Vec3* targetField;
} CheckerboardShader;

extern Shader* createCheckerboardShader(Vec3* color1, Vec3* color2, Vec2* scale, Vec2* bias,
        Shader* target, Vec3* targetField);

#endif /* CHECKERBOARD_H_ */
