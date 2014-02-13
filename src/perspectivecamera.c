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
    copy3(&camera->baseray.point, &ray->point);
    addscaled3(&camera->baseray.dir, u, &camera->du, &ray->dir);
    addscaled3(&ray->dir, v, &camera->dv, &ray->dir);
    normalize3(&ray->dir);
}

Camera* createPerspectiveCamera(Vec3* from, Vec3* at, Vec3* up, float fov, float aspect) {
    PerspectiveCamera* camera = (PerspectiveCamera*) malloc(sizeof(PerspectiveCamera));
    camera->op.makeRay = makeRay;
    float tanfov2 = 2.0f * tan(Radians(fov / 2.0f));
    Vec3 dir; sub3(at, from, &dir); normalize3(&dir);
    Vec3 upNormalized; copy3(up, &upNormalized); normalize3(&upNormalized);
    Vec3 tdu; cross(&dir, &upNormalized, &tdu); normalize3(&tdu); mult3(&tdu, tanfov2, &tdu);
    Vec3 tdv; cross(&tdu, &dir, &tdv); normalize3(&tdv); mult3(&tdv, tanfov2, &tdv);
    mult3(&tdu, aspect, &tdu);
    copy3(&tdu, &camera->du);
    copy3(&tdv, &camera->dv);
    copy3(from, &camera->baseray.point);
    addscaled3(&dir, -0.5f, &camera->du, &camera->baseray.dir); // camera->dir = dir - 0.5*du - 0.5*dv
    addscaled3(&camera->baseray.dir, -0.5f, &camera->dv, &camera->baseray.dir);
    return (Camera*) camera;
}




