/*
 * camera.h
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#ifndef CAMERA_H_
#define CAMERA_H_

typedef struct _Ray Ray;
typedef struct _Camera Camera;

typedef struct _CameraOps {
    void (*makeRay)(Camera*, float u, float v, Ray* ray);
} CameraOps;

typedef struct _Camera {
    CameraOps* op;
} Camera;

#endif /* CAMERA_H_ */
