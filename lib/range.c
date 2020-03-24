#include "range.h"
#include "util.h"

void range_init(Vec2* r) {
    vec2(RANGE_MIN, RANGE_MAX, r);
}

int range_is_empty(Vec2* r) {
    return r->x > r->y;
}

void range_intersect_floats(Vec2* r1, float t0, float t1) {
    r1->x = fmaxf(r1->x, t0);
    r1->y = fminf(r1->y, t1);
}

void range_intersect_ray_box(Vec2* range, Vec3* boxmin, Vec3* boxmax, Ray* ray) {
    float t0, t1;

    t0 = (boxmin->x - ray->point.x) / ray->dir.x;
    t1 = (boxmax->x - ray->point.x) / ray->dir.x;
    if(ray->dir.x >= 0.0f)
        range_intersect_floats(range, t0, t1);
    else
        range_intersect_floats(range, t1, t0);

    t0 = (boxmin->y - ray->point.y) / ray->dir.y;
    t1 = (boxmax->y - ray->point.y) / ray->dir.y;
    if(ray->dir.y >= 0.0f)
        range_intersect_floats(range, t0, t1);
    else
        range_intersect_floats(range, t1, t0);

    t0 = (boxmin->z - ray->point.z) / ray->dir.z;
    t1 = (boxmax->z - ray->point.z) / ray->dir.z;
    if(ray->dir.z >= 0.0f)
        range_intersect_floats(range, t0, t1);
    else
        range_intersect_floats(range, t1, t0);
}

