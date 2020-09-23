
#define _GNU_SOURCE
#include <math.h>
#include "raymath.h"

void copy_vect(struct vect *a, const struct vect *b) {
	a->x = b->x;
	a->y = b->y;
}

void add_vect(struct vect *a, const struct vect *b) {
	a->x += b->x;
	a->y += b->y;
}

void mult_vect(struct vect *a, const struct vect *b) {
	a->x *= b->x;
	a->y *= b->y;
}

void rotate_vect(double angle, struct vect *a) {
	struct vect result;
	double ang = d2r(angle);
	double sina, cosa;
	sincos(ang, &sina, &cosa);

	result.x = a->y * cosa;
	result.y = a->y * sina;
	
	sincos(ang - (PI / 2), &sina, &cosa);
	result.x += a->x * cosa;
	result.y += a->x * sina;

	copy_vect(a, &result);
}

double cotangent(double radians) {
	double s,c;
	sincos(radians, &s, &c);
	return c/s;
}
