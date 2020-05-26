/*
 * camera.h
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#ifndef CAMERA_H_
#define CAMERA_H_

typedef struct Camera Camera;

struct CameraOps {
    void (*makeRay)(Camera*, float u, float v, Ray* ray);
};

typedef struct CameraOps CameraOps;

struct Camera {
    CameraOps* op;
};

#endif /* CAMERA_H_ */
