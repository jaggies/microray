/*
 * perspectivecamera.h
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#ifndef PERSPECTIVECAMERA_H_
#define PERSPECTIVECAMERA_H_

#include "camera.h"
#include "vec3.h"
#include "ray.h"

typedef struct PerspectiveCamera {
    struct CameraOps* op;
    Vec3 du, dv; // orthogonal vectors define the camera plane
    Ray baseray; // ray originating at uv(0,0) pointing to lower left
} PerspectiveCamera;

extern Camera* createPerspectiveCamera(Vec3* from, Vec3* at, Vec3* up, float fov, float aspect);

#endif /* PERSPECTIVECAMERA_H_ */
