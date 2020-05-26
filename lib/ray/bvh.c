#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy()
#include <float.h>
#include <assert.h>
#ifdef PROFILE
#include <time.h>
#endif
#include "hit.h"
#include "shader.h"
#include "bvh.h"
#include "branch.h"
#include "leaf.h"
#include "util.h"

#define MAX_MAX_BVH_DEPTH 16
#define MAX_LEAF_SIZE_COUNT 16

static int bvh_max_depth = 8;
static unsigned int bvh_leaf_max = 4;

static const Vec3 SplitX = { 1.0f, 0.0f, 0.0f };
static const Vec3 SplitY = { 0.0f, 1.0f, 0.0f };
static const Vec3 SplitZ = { 0.0f, 0.0f, 1.0f };

static void initialize_BVH_parameters()
{
    if(getenv("BVH_MAX_DEPTH") != 0) {
        bvh_max_depth = atoi(getenv("BVH_MAX_DEPTH"));
        if(bvh_max_depth > MAX_MAX_BVH_DEPTH)
            bvh_max_depth = MAX_MAX_BVH_DEPTH;
        fprintf(stderr, "BVH max depth set to %d\n", bvh_max_depth);
    }
    if(getenv("BVH_LEAF_MAX") != 0) {
        bvh_leaf_max = atoi(getenv("BVH_LEAF_MAX"));
        fprintf(stderr, "BVH max shapes per leaf set to %u\n", bvh_leaf_max);
    }
}

#ifdef PROFILE
static int total_treed = 0;
static time_t previous;
static int bvh_level_counts[MAX_MAX_BVH_DEPTH];
/* Each element N of this array is the number of leafs with N children */
size_t bvh_leaf_size_counts[MAX_LEAF_SIZE_COUNT + 1];
size_t bvh_node_count = 0;
size_t bvh_branch_count = 0;
size_t bvh_leaf_count = 0;

void print_tree_stats()
{
    fprintf(stderr, "%lu bvh nodes\n", bvh_node_count);
    fprintf(stderr, "%lu of those are leaves\n", bvh_leaf_count);
    for(int i = 0; i < bvh_max_depth + 1; i++) {
        fprintf(stderr, "bvh level %2d: %6d nodes\n", i, bvh_level_counts[i]);
    }
    int largest_leaf_count = MAX_LEAF_SIZE_COUNT;
    while((largest_leaf_count > 0) && (bvh_leaf_size_counts[largest_leaf_count]) == 0)
        largest_leaf_count--;

    for(int i = 0; i <= largest_leaf_count; i++) {
        fprintf(stderr, "%2d shapes in %6lu leaves\n", i, bvh_leaf_size_counts[i]);
    }
    if(bvh_leaf_size_counts[MAX_LEAF_SIZE_COUNT] > 0)
        fprintf(stderr, "%d or more shapes in %6lu leaves\n", MAX_LEAF_SIZE_COUNT, bvh_leaf_size_counts[MAX_LEAF_SIZE_COUNT]);
}
#endif /* PROFILE */

const Vec3* sort_dir;
int shape_sort(const void *p1, const void *p2)
{
    int result;

    Shape* s1 = *(Shape**)p1;
    Shape* s2 = *(Shape**)p2;

    Vec3 s1boxmin;
    Vec3 s1boxmax;
    Vec3 s1center;
    Vec3 s2boxmin;
    Vec3 s2boxmax;
    Vec3 s2center;
    Vec3 diff;
	float dot;

    s1->op->bounds(s1, &s1boxmin, &s1boxmax);
    add3(&s1boxmin, &s1boxmax, &s1center);
    mult3(&s1center, .5f, &s1center);

    s2->op->bounds(s2, &s2boxmin, &s2boxmax);
    add3(&s2boxmin, &s2boxmax, &s2center);
    mult3(&s2center, .5f, &s2center);

    sub3(&s2center, &s1center, &diff);

    dot = dot3(&diff, sort_dir);

    if(dot < 0)
        result = 1; /* s1 should be after s2 */
    else if(dot > 0)
        result = -1; /* s1 should be before s2 */
    else
        result = 0; /* s1 is at s2 */

    return result;
}

// Creates a tree composed of Branches and Leaf nodes.
Shape* make_tree(Shape** shapes, int nShapes, int level)
{
    Vec3 boxmin, boxmax, boxdim;
    const Vec3* split_plane_normal;
    int i, startA, countA, startB, countB;
    Shape *g;

#ifdef PROFILE
    if(level == 0) {
        previous = time(NULL);
    }
#endif /* PROFILE */

#ifdef PROFILE
    if(time(NULL) > previous) {
        fprintf(stderr, "total treed = %d\n", total_treed);
        previous = time(NULL);
    }
#endif /* PROFILE */

    if((level >= bvh_max_depth) || nShapes <= bvh_leaf_max) {
        Shape* s = createLeaf(shapes, nShapes);
#ifdef PROFILE
        total_treed += nShapes;
        bvh_leaf_size_counts[(nShapes > MAX_LEAF_SIZE_COUNT) ? MAX_LEAF_SIZE_COUNT : nShapes]++;
        bvh_leaf_count++;
        bvh_level_counts[level]++;
        bvh_node_count++;
#endif /* PROFILE */
        return s;
    }

    /* find bounding box of all children */
    vec3(FLT_MAX, FLT_MAX, FLT_MAX, &boxmin);
    vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX, &boxmax);

    for(i = 0; i < nShapes; i++) {
        /* extend box by child box */
        Vec3 childmax;
        Vec3 childmin;

        shapes[i]->op->bounds(shapes[i], &childmin, &childmax);

        boxmin.x = fminf(boxmin.x, childmin.x);
        boxmin.y = fminf(boxmin.y, childmin.y);
        boxmin.z = fminf(boxmin.z, childmin.z);
        boxmax.x = fmaxf(boxmax.x, childmax.x);
        boxmax.y = fmaxf(boxmax.y, childmax.y);
        boxmax.z = fmaxf(boxmax.z, childmax.z);
    }

    sub3(&boxmax, &boxmin, &boxdim);

    if(boxdim.x > boxdim.y && boxdim.x > boxdim.z) {
        split_plane_normal = &SplitX;
    } else if(boxdim.y > boxdim.z) {
        split_plane_normal = &SplitY;
    } else {
        split_plane_normal = &SplitZ;
    }

    sort_dir = split_plane_normal;
    qsort(shapes, nShapes, sizeof(Shape*), shape_sort);

    startA = 0;
    countA = nShapes / 2;
    startB = startA + countA;
    countB = nShapes - countA;

    if(countA > 0 && countB > 0) {

        /* construct children */
        Shape *g1 = make_tree(shapes + startA, countA, level + 1);
        Shape *g2 = make_tree(shapes + startB, countB, level + 1);
        g = createBranch(g1, g2, split_plane_normal);
#ifdef PROFILE
        bvh_branch_count++;
#endif
    } else {

        g = createLeaf(shapes, nShapes);

#ifdef PROFILE
        total_treed += nShapes;
        bvh_leaf_size_counts[(nShapes > MAX_LEAF_SIZE_COUNT) ? MAX_LEAF_SIZE_COUNT : nShapes]++;
        bvh_leaf_count++;
#endif /* PROFILE */
    }

#ifdef PROFILE
        bvh_level_counts[level]++;
        bvh_node_count++;
#endif /* PROFILE */

    return g;
}

static
int bvhIntersect(Shape* shape, Ray* ray, Hit* hit) {
    BVH* bvh = (BVH*) shape;
    return bvh->root->op->intersect(bvh->root, ray, hit);
}

static
void bvhNormal(Shape* shape, Hit* hit, Vec3 *n) {
    BVH* bvh = (BVH*) shape;
    assert(0); // should never be called
    bvh->root->op->normal(bvh->root, hit, n);
}

static
void bvhUV(Shape* shape, Hit* hit, Vec2 * uv) {
    BVH* bvh = (BVH*) shape;
    assert(0); // should never be called
    bvh->root->op->uv(bvh->root, hit, uv);
}

static
void bvhBounds(Shape* shape, Vec3* min, Vec3* max) {
    BVH* bvh = (BVH*) shape;
    bvh->root->op->bounds(bvh->root, min, max); // TODO
}

static
void bvhDestroy(Shape* shape) {
    BVH* bvh = (BVH*) shape;
    size_t i;
    for (i = 0; i < bvh->nShapes; i++) {
        Shape* toDestroy = bvh->shapes[i];
        toDestroy->op->destroy(toDestroy);
    }
    free(bvh->shapes);
    bvh->root->op->destroy(bvh->root);
    free(bvh);
}

static ShapeOps _bvhOps;

// Returns a BVH object as the top level in the scene graph. This is a little
// wasteful, but allows hierarchical BVH nodes.
Shape *createBVH(Shape** shapes, int nShapes)
{
    BVH* bvh = (BVH*) malloc(sizeof(BVH));

    initialize_BVH_parameters();

	if (!_bvhOps.intersect) {
	    _bvhOps.intersect = bvhIntersect;
	    _bvhOps.normal = bvhNormal;
	    _bvhOps.uv = bvhUV;
	    _bvhOps.bounds = bvhBounds;
	    _bvhOps.destroy = bvhDestroy;
    }

	bvh->op = &_bvhOps;
	bvh->root = make_tree(shapes, nShapes, 0);

	// Note: these are only used for memory housekeeping.
	bvh->shapes = (Shape**) malloc(nShapes * sizeof(Shape*));
	memcpy(bvh->shapes, shapes, nShapes * sizeof(Shape*));
	bvh->nShapes = nShapes;

#ifdef PROFILE
    print_tree_stats();
#endif /* PROFILE */

    return (Shape*) bvh;
}

