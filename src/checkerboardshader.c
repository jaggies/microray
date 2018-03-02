/*
 * checkerboardshader.c
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#include <math.h>
#include "hit.h"
#include "checkerboardshader.h"

static void evaluate(struct Shader* sh, Hit* hit, Vec3* color)
{
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    Vec2 p; multadd2(&shader->scale, &hit->uv, &shader->bias, &p);
    int u = ((int) floorf(p.x)) & 1;
    int v = ((int) floorf(p.y)) & 1;
    shader->target = (u^v) ? shader->odd : shader->even;
    shader->target->op->evaluate(shader->target, hit, color);
}

static float getReflectionAmount(struct Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op->getReflectionAmount(shader->target);
}

static float getTransmissionAmount(struct Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op->getTransmissionAmount(shader->target);
}

static float getIndexOfRefraction(struct Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op->getIndexOfRefraction(shader->target);
}

static ShaderOps _checkerOps;

Shader* createCheckerboardShader(Shader* even, Shader* odd, Vec2* scale, Vec2* bias)  {
    if (!_checkerOps.evaluate) {
    	_checkerOps.evaluate = evaluate;
    	_checkerOps.getIndexOfRefraction = getIndexOfRefraction;
    	_checkerOps.getReflectionAmount = getReflectionAmount;
    	_checkerOps.getTransmissionAmount = getTransmissionAmount;
    }
    CheckerboardShader* shader = (CheckerboardShader*) malloc(sizeof(CheckerboardShader));
    shader->op = &_checkerOps;
    shader->even = even;
    shader->odd = odd;
    copy2(scale, &shader->scale);
    copy2(bias, &shader->bias);
    return (Shader*) shader;
}



