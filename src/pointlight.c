/*
 * pointlight.c
 *
 *  Created on: Feb 10, 2014
 *      Author: jmiller
 */

#include <stdlib.h>
#include "pointlight.h"

static void makeRay(Light* l, Vec3* point, Ray* ray)
{
    PointLight* light = (PointLight*) l;
    copy(point, &ray->point);
    sub(&light->point, point, &ray->dir);
    normalize(&ray->dir);
}

PointLight* createPointLight(Vec3* position, Vec3* color)
{
    PointLight* light = (PointLight*) malloc(sizeof(PointLight));
    copy(position, &light->point);
    copy(color, &light->color);
    light->op.makeRay = makeRay;
    return light;
}

