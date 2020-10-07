
#include <stdlib.h>
#include "level.h"
#include "render.h"
#include "input_defs.h"

void init_board(struct board *b, unsigned int w, unsigned int h) {
	b->w = w;
	b->h = h;
	b->tiles = malloc(sizeof(tile) * w * h);
}

tile tile_at(const struct board *b, unsigned int x, unsigned int y) {
	if (x >= b->w || y >= b->h) return 0;
	return b->tiles[x + b->w * y];
}

void set_tile(struct board *b, int x, int y, tile t) {
	b->tiles[x + b->w * y] = t;
}

void destroy_board(struct board *b) {
	b->w = 0;
	b->h = 0;
	free(b->tiles);
}

void init_player(struct player *p) {
	p->friction = FRICTION;
	p->speed = SPEED;
	p->accel = ACCEL;

	p->pos.x = 5;
	p->pos.y = 6;
}

void accelerate(double *x, struct player *p, int sign) {
	if (abs(*x) > p->speed)
		return;
	else *x += p->accel * sign;
}

void move(struct game *g) {
	struct player *p = &g->player;

	struct vect rotated, newpos;

	rotated = p->velocity;
	rotate_vect(p->angle, &rotated);
	newpos = p->pos;
	add_vect(&newpos, &rotated);


	if (tile_at(&g->board, round(newpos.x + 0.2), round(p->pos.y)) != 0) 
		newpos.x = round(newpos.x + 0.2) - 0.7;
	else if (tile_at(&g->board, round(newpos.x - 0.2), round(p->pos.y)) != 0) 
		newpos.x = round(newpos.x - 0.2) + 0.7;

	if (tile_at(&g->board, round(p->pos.x), round(newpos.y + 0.2)) != 0)
		newpos.y = round(newpos.y + 0.2) - 0.7;
	else if (tile_at(&g->board, round(p->pos.x), round(newpos.y - 0.2)) != 0)
		newpos.y = round(newpos.y - 0.2) + 0.7;

	p->pos = newpos;
}

void game_step(struct game *g) {
	struct player *p = &g->player;

	if (check_key(&i_move_forward))
		accelerate(&p->velocity.y, p, 1);
	if (check_key(&i_move_backward))
		accelerate(&p->velocity.y, p, -1);
	if (check_key(&i_strafe_left))
		accelerate(&p->velocity.x, p, 1);
	if (check_key(&i_strafe_right))
		accelerate(&p->velocity.x, p, -1);
	if (check_key(&i_look_left))
		p->angle -= check_key(&i_fast_look_mod) ? 8 : 3;
	if (check_key(&i_look_right))
		p->angle += check_key(&i_fast_look_mod) ? 8 : 3;

	move(g);

	p->velocity.x /= 1 + p->friction;
	p->velocity.y /= 1 + p->friction;

	if (p->angle < 0) p->angle += 360;
	if (p->angle >= 360) p->angle -= 360;

	static int debug_cool = 0;
	if (check_key(&i_debug) && debug_cool == 0) {
		debug_view = !debug_view;
		debug_cool = 10;
	} else if (debug_cool != 0) {
		debug_cool -= 1;
	}
}


