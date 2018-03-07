/*
 * branch.c
 *
 *  Created on: Mar 6, 2018
 *      Author: grantham@plunk.org
 */

#include <stdio.h>
#include <stdlib.h>
#include "branch.h"
#include "shader.h"
#include "hit.h"

void range_init(Vec2* r) {
    r->x = -100000000.0;
    r->y =  100000000.0;
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
    if(ray->dir.x >= 0.0)
        range_intersect_floats(range, t0, t1);
    else
        range_intersect_floats(range, t1, t0);

    t0 = (boxmin->y - ray->point.y) / ray->dir.y;
    t1 = (boxmax->y - ray->point.y) / ray->dir.y;
    if(ray->dir.y >= 0.0)
        range_intersect_floats(range, t0, t1);
    else
        range_intersect_floats(range, t1, t0);

    t0 = (boxmin->z - ray->point.z) / ray->dir.z;
    t1 = (boxmax->z - ray->point.z) / ray->dir.z;
    if(ray->dir.z >= 0.0)
        range_intersect_floats(range, t0, t1);
    else
        range_intersect_floats(range, t1, t0);
}

// XXX
extern long intersections;

static
int Branch_intersect(struct Shape* shape, Ray* ray, Hit* hit) {

    if(shape == hit->ignore)
        return 0;

    intersections++;

    Branch* branch = (Branch*) shape;

    BoundedShape* child0;
    BoundedShape* child1;

    if(dot3(&ray->dir, &branch->dir) > 0) {
        child0 = &branch->closerChild;
        child1 = &branch->fartherChild;
    } else {
        child0 = &branch->fartherChild;
        child1 = &branch->closerChild;
    }

    int result = 0;
    Vec2 range;

    range_init(&range);
    range_intersect_ray_box(&range, &child0->boxmin, &child0->boxmax, ray);
    if ((!range_is_empty(&range)) && (range.x < hit->t))
        result += child0->shape->op->intersect(child0->shape, ray, hit);

    range_init(&range);
    range_intersect_ray_box(&range, &child1->boxmin, &child1->boxmax, ray);
    if ((!range_is_empty(&range)) && (range.x < hit->t))
        result += child1->shape->op->intersect(child1->shape, ray, hit);

    return result;
}

static
void Branch_normal(struct Shape* shape, Hit* hit, Vec3 *n) {
    Branch* branch = (Branch*) shape;

    // ???
    // sub3(&hit->point, &sphere->position, n);

    normalize3(n);
}

static
void Branch_uv(struct Shape* shape, struct Hit* hit, Vec2 * uv) {
    uv->x = uv->y = 0.0f; // TODO
}

static
void Branch_bounds(struct Shape* shape, Vec3* boxmin, Vec3* boxmax) {
    Branch* branch = (Branch*) shape;

    BoundedShape *closer = &branch->closerChild;
    BoundedShape *farther = &branch->fartherChild;

    boxmin->x = fminf(closer->boxmin.x, farther->boxmin.x);
    boxmin->y = fminf(closer->boxmin.y, farther->boxmin.y);
    boxmin->z = fminf(closer->boxmin.z, farther->boxmin.z);
    boxmax->x = fmaxf(closer->boxmax.x, farther->boxmax.x);
    boxmax->y = fmaxf(closer->boxmax.y, farther->boxmax.y);
    boxmax->z = fmaxf(closer->boxmax.z, farther->boxmax.z);
}

static ShapeOps _BranchOps;

Shape* createBranch(Shape* closer, Shape* farther, Vec3 *dir) {
    Branch* branch = (Branch*) malloc(sizeof(Branch));

    if (!_BranchOps.intersect) {
        _BranchOps.intersect = Branch_intersect;
        _BranchOps.normal = Branch_normal;
        _BranchOps.uv = Branch_uv;
        _BranchOps.bounds = Branch_bounds;
    }
    branch->op = &_BranchOps;

    branch->closerChild.shape = closer;
    closer->op->bounds(closer, &branch->closerChild.boxmin, &branch->closerChild.boxmax); 

    branch->fartherChild.shape = farther;
    farther->op->bounds(farther, &branch->fartherChild.boxmin, &branch->fartherChild.boxmax); 

    branch->dir = *dir;

    return (Shape*) branch;
}
