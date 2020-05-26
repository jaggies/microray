/*
 * checkerboard.h
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#ifndef CHECKERBOARD_H_
#define CHECKERBOARD_H_

#include "vec2.h"
#include "vec3.h"

typedef struct CheckerboardShader CheckerboardShader;

struct CheckerboardShader {
    ShaderOps* op;
    Shader* odd;
    Shader* even;
    Vec2 scale;
    Vec2 bias;
    /* private */
    Shader* target; /* which of the above shaders is being evaluated */
};

extern Shader* createCheckerboardShader(Shader* odd, Shader* even, Vec2* scale, Vec2* bias);

#endif /* CHECKERBOARD_H_ */
