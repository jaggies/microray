/*
 * checkerboardshader.c
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#include <math.h>
#include <stdlib.h>
#include "hit.h"
#include "shader.h"
#include "checkerboardshader.h"
#include "util.h"

static void checkerEvaluate(Shader* sh, Hit* hit, Vec3* color)
{
	int u, v;
    Vec2 p; 
    CheckerboardShader* shader = (CheckerboardShader*) sh;
	multadd2(&shader->scale, &hit->uv, &shader->bias, &p);
    u = ((int) floorf(p.x)) & 1;
    v = ((int) floorf(p.y)) & 1;
    shader->target = (u^v) ? shader->odd : shader->even;
    (*shader->target->op->evaluate)(shader->target, hit, color);
}

static float checkerGetReflectionAmount(Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op->getReflectionAmount(shader->target);
}

static float checkerGetTransmissionAmount(Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op->getTransmissionAmount(shader->target);
}

static float checkerGetIndexOfRefraction(Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op->getIndexOfRefraction(shader->target);
}

static ShaderOps _checkerOps;

Shader* createCheckerboardShader(Shader* even, Shader* odd, Vec2* scale, Vec2* bias)  {
    CheckerboardShader* shader = (CheckerboardShader*) malloc(sizeof(CheckerboardShader));
    if (!_checkerOps.evaluate) {
        _checkerOps.evaluate = checkerEvaluate;
        _checkerOps.getIndexOfRefraction = checkerGetIndexOfRefraction;
        _checkerOps.getReflectionAmount = checkerGetReflectionAmount;
        _checkerOps.getTransmissionAmount = checkerGetTransmissionAmount;
    }
    shader->op = &_checkerOps;
    shader->even = even;
    shader->odd = odd;
    copy2(scale, &shader->scale);
    copy2(bias, &shader->bias);
    return (Shader*) shader;
}

