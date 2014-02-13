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
    copy3(point, &ray->point);
    sub3(&light->point, point, &ray->dir);
    normalize3(&ray->dir);
}

Light* createPointLight(Vec3* position, Vec3* color)
{
    PointLight* light = (PointLight*) malloc(sizeof(PointLight));
    copy3(position, &light->point);
    copy3(color, &light->color);
    light->op.makeRay = makeRay;
    return (Light*) light;
}

