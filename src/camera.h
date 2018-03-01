/*
 * camera.h
 *
 *  Created on: Feb 12, 2014
 *      Author: jmiller
 */

#ifndef CAMERA_H_
#define CAMERA_H_

struct Ray;
struct Camera;

typedef struct CameraOps {
    void (*makeRay)(struct Camera*, float u, float v, struct Ray* ray);
} CameraOps;

typedef struct Camera {
    struct CameraOps* op;
} Camera;

#endif /* CAMERA_H_ */
