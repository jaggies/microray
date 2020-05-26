/*
 * perspectivecamera.c
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#include <stdlib.h>
#include "ray.h"
#include "perspcam.h"

static void perspMakeRay(Camera* cam, float u, float v, Ray* ray) {
    PerspectiveCamera* camera = (PerspectiveCamera*) cam;
    copy3(&camera->baseray.point, &ray->point);
    addscaled3(&camera->baseray.dir, u, &camera->du, &ray->dir);
    addscaled3(&ray->dir, v, &camera->dv, &ray->dir);
    normalize3(&ray->dir);
}

static CameraOps _perspCameraOps;

Camera* createPerspectiveCamera(Vec3* from, Vec3* at, Vec3* up, float fov, float aspect) {
    PerspectiveCamera* camera;
    Vec3 dir, upNormalized, tdu, tdv;
    float tanfov2;
    if (!_perspCameraOps.makeRay) {
        _perspCameraOps.makeRay = perspMakeRay;
    }
    camera = (PerspectiveCamera*) malloc(sizeof(PerspectiveCamera));
    tanfov2 = 2.0f * tan(Radians(fov / 2.0f));
    camera->op = &_perspCameraOps;
    sub3(at, from, &dir); normalize3(&dir);
    copy3(up, &upNormalized); normalize3(&upNormalized);
    cross(&dir, &upNormalized, &tdu); normalize3(&tdu); mult3(&tdu, tanfov2, &tdu);
    cross(&tdu, &dir, &tdv); normalize3(&tdv); mult3(&tdv, tanfov2, &tdv);
    mult3(&tdu, aspect, &tdu);
    copy3(&tdu, &camera->du);
    copy3(&tdv, &camera->dv);
    copy3(from, &camera->baseray.point);

    /* camera->dir = dir - 0.5*du - 0.5*dv */
    addscaled3(&dir, -0.5f, &camera->du, &camera->baseray.dir);
    addscaled3(&camera->baseray.dir, -0.5f, &camera->dv, &camera->baseray.dir);
    return (Camera*) camera;
}

