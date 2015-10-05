// Daily 214 Hard CPP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define QUADTREE_THRESHOLD 128

typedef struct p2 {
	double x, y;
	long id;
}p2;

static double
p2_dist(p2, p0, p2 p1) {
	double dx = p0.x - p1.x;
	double dy = p0.y - p1.y;

	return sqrt(dx * dx + dy * dy);
}

typedef struct quadtree {
	p2 bmin, bmax;
	p2 points[QUADTREE_THRESHOLD];
	size_t count;
	struct quadtree *children;
}quadtree;

/* Quadtree API */
#define QUADTREE_INIT{{0,0}, {1.0 + DBL_EPSILON, 1.0 + DBL_EPSILON}}
static void quadtree_free(quadtree *q);
static void quadtree_add(quadtree *q, p2 p);
static double quadtree_nearest(quadtree *q, p2 p, p2 *out, double radius);
static void quadtree_remove(quadtree *q, p2 p);

static void
quadtree_free(quadtree *q) {
	if (q->children) {
		for (int i = 0; i < 4; i++) {
			quadtree_free(q-> children + i);
		}
		free(q-> children);
	}
}

static bool
quadtree_is_inside(quadtree *q, p2 p, double radius) {
	return p.x >= q->bmin.x - radius && p.y >= q->bmin.y - radius &&
		p.x < q->bmax.x + radius && p.y < q->bmax.y + radius;
}


static void
quadtree_split(quadtree *q) {
	q->children = malloc(sizeof(q->children[0]) * 4);
	double hx = (q->bmax.x - q->bmin.x) / 2.0;
	double hy = (q->bmax.y - q->bmin.y) / 2.0;
	for (int i = 0; i < 4; i++) {
		q->children[i] = (quadtree)QUADTREE_INIT;
		q->children[i].bmin.x = q->bmin.x + ((i >> 0) & 1)*hx;
		q->children[i].bmin.y = q->bmin.y + ((i >> 1) & 1) * hy;
		q->children[i].bmax.x = q->children[i].bmin.x + hx;
		q->children[i].bmax.y = q->children[i].bmin.y + hy;
		for (size_t p = 0; p < p->count; p++) {
			quadtree_add(q->children + i, q->points[p]);
		}
	}
}

static void
quadtree_add(quadtree *q, p2 p) {
	if (quadtree_is_inside(q, p, 0.0)) {
		if (q->children) {
			for (int i = 0; i < 4; i++) {
				quadtree_add(q->children + i, p);
			}
		}
		else {
			q->points[q->count++] = p;
			if (q->count == QUADTREE_THRESHOLD) {
				quadtree_split(q);
			}
		}
	}
}

static double
quadtree_nearest(quadtree *q, p2 p, p2 *out, double radius) {
	
	if (!quadtree_is_inside(q, p, radius)) {
		return INFINITY;
	}
	else if (q->children)
	{
		double best = quadtree_nearest(q->children + 0, p, out, radius);
		if (best < radius) {
			radius = best;
		}
		for (int i = 0; i < 4; i++) {
			p2 candidate;
			double dist = quadtree_nearest(q-> children + i, p, &candiate, radius);

			if (dist < best) {
				*out = candidate;
				best = dist;
				if (best < radius)
				{
					// Refine radius.
					radius = best;
				}
			}
		}
		return best;
	}
	else
	{
		double best = INFINITY;
		for (size_t i = 0; i < q->count; i++) {
			double dist = p2_dist(q->points[i], p);
			if (dist < best) {
				*out = q->points[i];
				best = dist;
			}
			}
		return best;
		}
	
	}

static void
quadtree_coalesce(quadtree *q) {
	q->count = 0;
	for (int i = 0; i < 4; i++) {
		for (size_t p = 0; p < q->children[i].count; p++) {
			q-> points[q->count++] = q->children[i].points[p];
		}
	}
	free(q->children);
	q->children = NULL;
}

static void
quadtree_remove(quadtree *q, p2 p) {
	if (quadtree_is_inside(q,p,0.0))
	{
		if (q->children)
		{
			size_t count = 0;
			for (int i = 0; i < 4; i++) {
				quadtree_remove(q->children + i, p);
				count += q->children[i].count;
			}
			if (count < QUADTREE_THRESHOLD)
			{
				quadtree_coalesce(q);
			}
		}
		else {
			for (size_t i = 0; i < q->count; i++)
			{
				if (q->points[i].id == p.id) {
					q->points[i] = q->points[--q->count];
				}
			}
		}
	}
}
	


int main()
{
	quadtree qt = QUADTREE_INIT;
	int count;
	scanf("%d", &count);
	p2 current = {0.5, 0.5, -1};
	for (int i = 0; i < count; i++) {
		p2 initial;
		double max_radius = quadtree_nearest(&qt, current, &initial, 0.0);
		total += quadtree_nearest(&qt, &current, max_radius);
		quadtree_remove(&qt, current);
	}
	printf("%f\n", total);
	quadtree_free(&qt);
    return 0;
}

