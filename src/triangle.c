/*
 * triangle.c
 *
 *  Created on: Feb 11, 2014
 *      Author: jmiller
 */
#include "triangle.h"
#include "hit.h"

static float tmin = 0.0f;

struct TriangleHit {
    float alpha;
    float beta;
};

// TODO: move this to Hit data structure scratchpad
static struct TriangleHit hitData;

static
int intersect(struct Shape* shape, Ray* ray, float *tmax) {
    Triangle* triangle = (Triangle*) shape;

    Vec3 s1; cross(&ray->dir, &triangle->edge[1], &s1);
    float div = dot3(&s1, &triangle->edge[0]);
    if (div == 0.0f) return 0;  // ray parallel to plane.

    float invDiv = 1.0f / div;
    Vec3 d; sub3(&ray->point, &triangle->point[0], &d);

    // Compute barycentric coordinate 1
    float alpha = dot3(&d, &s1) * invDiv;
    if ((alpha < 0.0f) || (alpha > 1.0f)) return 0;

    // Compute barycentric coordinate 2
    Vec3 s2; cross(&d, &triangle->edge[0], &s2);
    float beta = dot3(&ray->dir, &s2) * invDiv;
    if ((beta < 0.0f) || ((alpha + beta) > 1.0f)) return 0;

    float t = dot3(&triangle->edge[1], &s2) * invDiv;

    if ((t > tmin) && (t < *tmax)) {
        *tmax = t;
        hitData.alpha = alpha;
        hitData.beta = beta;
        return 1;
    }
    return 0;
}

static
void normal(struct Shape* shape, Hit* hit, Vec3 *n) {
    copy3(&((Triangle*) shape)->normal[0], n);
}

static
void uv(struct Shape* shape, struct Hit* hit, Vec2 * uv) {
    Triangle* triangle = (Triangle*) shape;
    Vec2 edge10; sub2(triangle->uv + 1, triangle->uv + 0, &edge10);
    Vec2 edge20; sub2(triangle->uv + 2, triangle->uv + 0, &edge20);
    addscaled2(triangle->uv + 0, hitData.alpha, &edge10, uv);
    addscaled2(uv, hitData.beta, &edge20, uv);
}

Shape* createTriangle(
        Vec3* p0, Vec3* p1, Vec3* p2,
        Vec2* uv0, Vec2* uv1, Vec2* uv2,
        struct Shader* shader) {
    Triangle* triangle = (Triangle*) malloc(sizeof(Triangle));
    triangle->op.intersect = intersect;
    triangle->op.normal = normal;
    triangle->op.uv = uv;
    triangle->op.shader = shader;
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
