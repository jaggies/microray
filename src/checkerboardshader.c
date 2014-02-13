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
    copy3((u^v) ? &shader->color1 : &shader->color2, shader->targetField);
    shader->target->op.evaluate(shader->target, hit, color);
}

float getReflectionAmount(struct Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op.getReflectionAmount(shader->target);
}

float getTransmissionAmount(struct Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op.getTransmissionAmount(shader->target);
}

float getIndexOfRefraction(struct Shader* sh) {
    CheckerboardShader* shader = (CheckerboardShader*) sh;
    return shader->target->op.getIndexOfRefraction(shader->target);
}

Shader* createCheckerboardShader(Vec3* color1, Vec3* color2, Vec2* scale, Vec2* bias, Shader* target,
        Vec3* targetField) {
    CheckerboardShader* shader = (CheckerboardShader*) malloc(sizeof(CheckerboardShader));
    shader->op.evaluate = evaluate;
    shader->op.getIndexOfRefraction = getIndexOfRefraction;
    shader->op.getReflectionAmount = getReflectionAmount;
    shader->op.getTransmissionAmount = getTransmissionAmount;
    copy2(scale, &shader->scale);
    copy2(bias, &shader->bias);
    copy3(color1, &shader->color1);
    copy3(color2, &shader->color2);
    shader->target = target;
    shader->targetField = targetField;
    return (Shader*) shader;
}



