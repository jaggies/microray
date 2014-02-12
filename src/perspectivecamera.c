/*
 * perspectivecamera.c
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#include <stdlib.h>
#include "perspectivecamera.h"

static void makeRay(struct Camera* cam, float u, float v, Ray* ray) {
    PerspectiveCamera* camera = (PerspectiveCamera*) cam;
    copy(&camera->baseray.point, &ray->point);
    addscaled(&camera->baseray.dir, u, &camera->du, &ray->dir);
    addscaled(&ray->dir, v, &camera->dv, &ray->dir);
    normalize(&ray->dir);
}

Camera* createPerspectiveCamera(Vec3* from, Vec3* at, Vec3* up, float fov, float aspect) {
    PerspectiveCamera* camera = (PerspectiveCamera*) malloc(sizeof(PerspectiveCamera));
    camera->op.makeRay = makeRay;
    float tanfov2 = 2.0f * tan(Radians(fov / 2.0f));
    Vec3 dir; sub(at, from, &dir); normalize(&dir);
    Vec3 upNormalized; copy(up, &upNormalized); normalize(&upNormalized);
    Vec3 tdu; cross(&dir, &upNormalized, &tdu); normalize(&tdu); mult(&tdu, tanfov2, &tdu);
    Vec3 tdv; cross(&tdu, &dir, &tdv); normalize(&tdv); mult(&tdv, tanfov2, &tdv);
    mult(&tdu, aspect, &tdu);
    copy(&tdu, &camera->du);
    copy(&tdv, &camera->dv);
    copy(from, &camera->baseray.point);
    addscaled(&dir, -0.5f, &camera->du, &camera->baseray.dir); // camera->dir = dir - 0.5*du - 0.5*dv
    addscaled(&camera->baseray.dir, -0.5f, &camera->dv, &camera->baseray.dir);
    return (Camera*) camera;
}




