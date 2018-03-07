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

static
int Branch_intersect(struct Shape* shape, Ray* ray, Hit* hit) {

    if(shape == hit->ignore)
        return 0;

    Branch* branch = (Branch*) shape;

    BoundedShape* child0;
    BoundedShape* child1;

    if(dot3(&ray->dir, &branch->dir) > 0) {
        child0 = &branch->closerChild;
        child1 = &branch->fartherChild;
    } else {
        child1 = &branch->closerChild;
        child0 = &branch->fartherChild;
    }

    int hitsBox;
    int result = 0;

    // XXX hitsBox = child0 box intersect 
    if (hitsBox)
        result += child0->shape->op->intersect(child0->shape, ray, hit);

    // XXX hitsBox = child1 box intersect 
    if (hitsBox) 
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
    boxmin->x = fminf(branch->closerChild.boxmin.x, branch->fartherChild.boxmin.x);
    boxmin->y = fminf(branch->closerChild.boxmin.y, branch->fartherChild.boxmin.y);
    boxmin->z = fminf(branch->closerChild.boxmin.z, branch->fartherChild.boxmin.z);
    boxmax->x = fmaxf(branch->closerChild.boxmax.x, branch->fartherChild.boxmax.x);
    boxmax->y = fmaxf(branch->closerChild.boxmax.y, branch->fartherChild.boxmax.y);
    boxmax->z = fmaxf(branch->closerChild.boxmax.z, branch->fartherChild.boxmax.z);
}

static ShapeOps _BranchOps;

Shape* createBranch(Shape* closer, Shape* farther) {
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

    return (Shape*) branch;
}
