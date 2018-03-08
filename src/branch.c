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
#include "range.h"

#ifdef PROFILE
extern long intersections;
#endif /* PROFILE */

static
int Branch_intersect(struct Shape* shape, Ray* ray, Hit* hit) {

    if(shape == hit->ignore)
        return 0;

#ifdef PROFILE
    intersections++;
#endif /* PROFILE */

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

    // empty - never called
}

static
void Branch_uv(struct Shape* shape, struct Hit* hit, Vec2 * uv) {

    // empty - never called
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