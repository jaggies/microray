/*
 * triangle.h
 *
 *  Created on: Feb 11, 2014
 *      Author: jmiller
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "shape.h"

typedef struct Triangle Triangle;

struct Triangle {
    ShapeOps* op;
    const Shader* shader;
    Vec3  point[1];
    Vec3  normal[1];
    Vec3  edge[2];
    Vec2  uv[3];
};

extern Shape* createTriangle(const Vec3* p0, const Vec3* p1, const Vec3* p2,
        const Vec2* uv0, const Vec2* uv1, const Vec2* uv2, const Shader* shader);

#endif /* TRIANGLE_H_ */
