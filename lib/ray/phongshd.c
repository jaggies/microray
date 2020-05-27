/*
 * phongshader.c
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#include <stdlib.h>
#include "hit.h"
#include "vec3.h"
#include "phongshd.h"

static const float _threshold = 0.01f; /* minimum factor before we'll call pow() */

static void phongEvaluate(Shader* sh, Hit* hit, Vec3* color) {
    PhongShader* shader = (PhongShader*) sh;
    float cosAlpha, cosBeta;

    /* Ambient */
    copy3(&shader->ambient, color);

    if (hit->inShadow) return;

    /* Diffuse */
    cosAlpha = dot3(&hit->normal, &hit->lightRay.dir);
    cosAlpha = fabs(cosAlpha);
    if (cosAlpha > 0) {
        addscaled3(color, cosAlpha, &shader->diffuse, color);
    }

    /* Specular */
    cosBeta = dot3(&hit->reflect, &hit->lightRay.dir);
    if (cosBeta > _threshold) {
        float ks = pow(cosBeta, shader->exponent);
        addscaled3(color, ks, &shader->specular, color);
    }
}

static float phongGetReflectionAmount(Shader* sh) {
    return ((PhongShader*) sh)->reflect;
}

static float phongGetTransmissionAmount(Shader* sh) {
    return ((PhongShader*) sh)->transmit;
}

static float phongGetIndexOfRefraction(Shader* sh) {
    return ((PhongShader*) sh)->index;
}

static ShaderOps _phongOps;

PhongShader* createPhongShader(Vec3* diffuse, Vec3* specular, Vec3* ambient, float exponent, float index,
        float reflect, float transmit) {
    PhongShader* shader = (PhongShader*) malloc(sizeof(PhongShader));
    if (!_phongOps.evaluate) {
        _phongOps.evaluate = phongEvaluate;
        _phongOps.getReflectionAmount = phongGetReflectionAmount;
        _phongOps.getTransmissionAmount = phongGetTransmissionAmount;
        _phongOps.getIndexOfRefraction = phongGetIndexOfRefraction;
    }
    shader->op = &_phongOps;
    copy3(diffuse, &shader->diffuse);
    copy3(specular, &shader->specular);
    copy3(ambient, &shader->ambient);
    shader->exponent = exponent;
    shader->index = index;
    shader->reflect = reflect;
    shader->transmit = transmit;
    return shader;
}
