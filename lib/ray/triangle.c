/*
 * triangle.c
 *
 *  Created on: Feb 11, 2014
 *      Author: jmiller
 */
#include <stdlib.h>
#include "hit.h"
#include "shader.h"
#include "triangle.h"
#include "util.h"

#define tmin (0.0f)

typedef struct _TriangleHit {
    float alpha;
    float beta;
} TriangleHit;

/* TODO: move this to Hit data structure scratchpad */
static TriangleHit hitData;

#ifdef PROFILE
extern long intersections;
#endif /* PROFILE */

static
int triangleIntersect(Shape* shape, Ray* ray, Hit *hit) {
    Triangle* triangle;
    float div, invDiv, alpha, beta, t;
    Vec3 d, s1, s2;

    if(shape == hit->ignore)
        return 0;

#ifdef PROFILE
    intersections++;
#endif /* PROFILE */

    triangle = (Triangle*) shape;

    cross(&ray->dir, &triangle->edge[1], &s1);
    div = dot3(&s1, &triangle->edge[0]);
    if (div == 0.0f) return 0;  /* ray parallel to plane. */

    invDiv = 1.0f / div;
    sub3(&ray->point, &triangle->point[0], &d);

    /* Compute barycentric coordinate 1 */
    alpha = dot3(&d, &s1) * invDiv;
    if ((alpha < 0.0f) || (alpha > 1.0f)) return 0;

    /* Compute barycentric coordinate 2 */
    cross(&d, &triangle->edge[0], &s2);
    beta = dot3(&ray->dir, &s2) * invDiv;
    if ((beta < 0.0f) || ((alpha + beta) > 1.0f)) return 0;

    t = dot3(&triangle->edge[1], &s2) * invDiv;

    if ((t > tmin) && (t < hit->t)) {
        hit->t = t;
        hit->best = shape;
        hitData.alpha = alpha;
        hitData.beta = beta;
        return 1;
    }
    return 0;
}

static
void triangleNormal(Shape* shape, Hit* hit, Vec3 *n) {
    copy3(&((Triangle*) shape)->normal[0], n);
}

static void triangleBounds(Shape* shape, Vec3* min, Vec3* max) {
    Triangle* triangle = (Triangle*) shape;
    Vec3 point1, point2;

    *max = triangle->point[0];
    *min = triangle->point[0];

    add3(&triangle->point[0], &triangle->edge[0], &point1);
    min->x = fminf(min->x, point1.x);
    min->y = fminf(min->y, point1.y);
    min->z = fminf(min->z, point1.z);
    max->x = fmaxf(max->x, point1.x);
    max->y = fmaxf(max->y, point1.y);
    max->z = fmaxf(max->z, point1.z);

    add3(&triangle->point[0], &triangle->edge[1], &point2);
    min->x = fminf(min->x, point2.x);
    min->y = fminf(min->y, point2.y);
    min->z = fminf(min->z, point2.z);
    max->x = fmaxf(max->x, point2.x);
    max->y = fmaxf(max->y, point2.y);
    max->z = fmaxf(max->z, point2.z);
}

static
void triangleUV(Shape* shape, Hit* hit, Vec2 * uv) {
    Triangle* triangle = (Triangle*) shape;
    Vec2 edge10, edge20;
    sub2(triangle->uv + 1, triangle->uv + 0, &edge10);
    sub2(triangle->uv + 2, triangle->uv + 0, &edge20);
    addscaled2(triangle->uv + 0, hitData.alpha, &edge10, uv);
    addscaled2(uv, hitData.beta, &edge20, uv);
}

static
void triangleDestroy(Shape* shape) {
    Triangle* triangle = (Triangle*) shape;
    free(triangle);
}

static ShapeOps _triangleOps;

Shape* createTriangle(
        Vec3* p0, Vec3* p1, Vec3* p2,
        Vec2* uv0, Vec2* uv1, Vec2* uv2,
        Shader* shader) {
    Triangle* triangle = (Triangle*) malloc(sizeof(Triangle));
    if (!_triangleOps.intersect) {
        _triangleOps.intersect = triangleIntersect;
        _triangleOps.normal = triangleNormal;
        _triangleOps.uv = triangleUV;
        _triangleOps.bounds = triangleBounds;
        _triangleOps.destroy = triangleDestroy;
    }
    triangle->op = &_triangleOps;
    triangle->shader = shader;
    copy3(p0, &triangle->point[0]);
    sub3(p1, p0, &triangle->edge[0]);
    sub3(p2, p0, &triangle->edge[1]);
    copy2(uv0, &triangle->uv[0]);
    copy2(uv1, &triangle->uv[1]);
    copy2(uv2, &triangle->uv[2]);
    cross(&triangle->edge[0], &triangle->edge[1], &triangle->normal[0]);
    normalize3(&triangle->normal[0]);
    return (Shape*) triangle;
}
