
#ifndef __RAYMATH_H
#define __RAYMATH_H

#define PI 3.141592653589793
static inline double d2r(double angle) {
	return angle * (PI * 2 / 360);
}
static inline double r2d(double angle) {
	return angle / (PI * 2 / 360);
}

static inline double sqr(double x) {
	return x*x;
}

struct vect {
	double x;
	double y;
};

void add_vect(struct vect *a, const struct vect *b);
void mult_vect(struct vect *a, const struct vect *b);
void rotate_vect(double angle, struct vect *a);

double cotangent(double radians);

#endif
