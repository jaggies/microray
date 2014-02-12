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

    Vec3 s1; cross(&ray->dir, &triangle->edge2, &s1);
    float div = dot(&s1, &triangle->edge1);
    if (div == 0.0f) return 0;  // ray parallel to plane.

    float invDiv = 1.0f / div;
    Vec3 d; sub(&ray->point, &triangle->point0, &d);

    // Compute barycentric coordinate 1
    float alpha = dot(&d, &s1) * invDiv;
    if ((alpha < 0.0f) || (alpha > 1.0f)) return 0;

    // Compute barycentric coordinate 2
    Vec3 s2; cross(&d, &triangle->edge1, &s2);
    float beta = dot(&ray->dir, &s2) * invDiv;
    if ((beta < 0.0f) || ((alpha + beta) > 1.0f)) return 0;

    float t = dot(&triangle->edge2, &s2) * invDiv;

    if ((t > tmin) && (t < *tmax)) {
        *tmax = t;
        hitData.alpha = alpha;
        hitData.beta = beta;
        return 1;
    }
    return 0;
}

static
void normal(struct Shape* shape, Ray* ray, Hit* hit, Vec3 *n) {
    copy(&((Triangle*) shape)->normal0, n);
}

Shape* createTriangle(Vec3* p0, Vec3* p1, Vec3* p2, struct Shader* shader) {
    Triangle* triangle = (Triangle*) malloc(sizeof(Triangle));
    triangle->op.intersect = intersect;
    triangle->op.normal = normal;
    triangle->op.shader = shader;
    copy(p0, &triangle->point0);
    sub(p1, p0, &triangle->edge1);
    sub(p2, p0, &triangle->edge2);
    cross(&triangle->edge1, &triangle->edge2, &triangle->normal0);
    normalize(&triangle->normal0);
    return (Shape*) triangle;
}
