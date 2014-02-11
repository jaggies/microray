/*
 * phongshader.c
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#include "phongshader.h"
#include "hit.h"

static float _threshold = 0.01f; // minimum factor before we'll call pow

static void evaluate(struct Shader* sh, Hit* hit, Vec3* color) {
    PhongShader* shader = (PhongShader*) sh;

    // Diffuse
    float cosAlpha = dot(&hit->normal, &hit->lightRay.dir);
    if (cosAlpha > 0) {
        mult(&shader->diffuse, cosAlpha, color);
    } else {
        makeVec3(0,0,0,color);
    }

    // Specular
    float cosBeta = dot(&hit->reflect, &hit->lightRay.dir);
    if (cosBeta > _threshold) {
        float ks = pow(cosBeta, shader->exponent);
        Vec3 specular;
        mult(&shader->specular, ks, &specular);
        add(&specular, color, color);
    }

    // Ambient
    add(&shader->ambient, color, color);
}

static float getReflectionAmount(struct Shader* sh) {
    return ((PhongShader*) sh)->reflect;
}

static float getTransmissionAmount(struct Shader* sh) {
    return ((PhongShader*) sh)->transmit;
}

static float getIndexOfRefraction(struct Shader* sh) {
    return ((PhongShader*) sh)->index;
}

Shader* createPhongShader(Vec3* diffuse, Vec3* specular, Vec3* ambient, float exponent, float index,
        float reflect, float transmit) {
    PhongShader* shader = (PhongShader*) malloc(sizeof(PhongShader));
    shader->op.evaluate = evaluate;
    shader->op.getReflectionAmount = getReflectionAmount;
    shader->op.getTransmissionAmount = getTransmissionAmount;
    shader->op.getIndexOfRefraction = getIndexOfRefraction;
    copy(diffuse, &shader->diffuse);
    copy(specular, &shader->specular);
    copy(ambient, &shader->ambient);
    shader->exponent = exponent;
    shader->index = index;
    shader->reflect = reflect;
    shader->transmit = transmit;
    return (Shader*) shader;
}
