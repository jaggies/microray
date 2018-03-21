/*
 * leaf.c
 *
 *  Created on: Mar 6, 2018
 *      Author: grantham@plunk.org
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include "leaf.h"
#include "shader.h"
#include "hit.h"

#ifdef PROFILE
extern long intersections;
#endif /* PROFILE */

static
int Leaf_intersect(Shape* shape, Ray* ray, Hit* hit) {
    Leaf* leaf;
    int s, result;

    if(shape == hit->ignore)
        return 0;

#ifdef PROFILE
    intersections++;
#endif /* PROFILE */

    leaf = (Leaf*) shape;
    result = 0;

    for (s = 0; s < leaf->nShapes; s++) {
        Shape* shape = leaf->shapes[s];
        result += shape->op->intersect(shape, ray, hit);
    }

    return result;
}

static
void Leaf_normal(Shape* shape, Hit* hit, Vec3 *n) {

    /* empty - never called */
}

static
void Leaf_uv(Shape* shape, Hit* hit, Vec2 * uv) {

    /* empty - never called */
}

static
void Leaf_bounds(Shape* shape, Vec3* boxmin, Vec3* boxmax) {
    Leaf* leaf = (Leaf*) shape;
	int s;

    /* make empty box */
    vec3(FLT_MAX, FLT_MAX, FLT_MAX, boxmin);
    vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX, boxmax);

    for(s = 0; s < leaf->nShapes; s++) {
        Vec3 childboxmin;
        Vec3 childboxmax;

        leaf->shapes[s]->op->bounds(leaf->shapes[s], &childboxmin, &childboxmax);

        /* extend box by child box */
        boxmin->x = fminf(boxmin->x, childboxmin.x);
        boxmin->y = fminf(boxmin->y, childboxmin.y);
        boxmin->z = fminf(boxmin->z, childboxmin.z);
        boxmax->x = fmaxf(boxmax->x, childboxmax.x);
        boxmax->y = fmaxf(boxmax->y, childboxmax.y);
        boxmax->z = fmaxf(boxmax->z, childboxmax.z);
    }
}

static ShapeOps _LeafOps;

Shape* createLeaf(Shape** shapes, int nShapes) {
    Leaf* leaf;

	/* assert(nShapes > 0); */

    leaf = (Leaf*) malloc(sizeof(Leaf));

    if (!_LeafOps.intersect) {
        _LeafOps.intersect = Leaf_intersect;
        _LeafOps.normal = Leaf_normal;
        _LeafOps.uv = Leaf_uv;
        _LeafOps.bounds = Leaf_bounds;
    }
    leaf->op = &_LeafOps;

    leaf->shapes = (Shape**)malloc(sizeof(Shape*) * nShapes);
    memcpy(leaf->shapes, shapes, sizeof(Shape*) * nShapes);
    leaf->nShapes = nShapes;

    return (Shape*) leaf;
}
