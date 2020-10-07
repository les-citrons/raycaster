
#ifndef LEVEL_H
#define LEVEL_H

#include "raymath.h"

struct player {
	struct vect pos;
	struct vect velocity;
	float friction;
	float speed;
	float accel;
	double angle;
};

typedef unsigned short tile;

struct board {
	tile *tiles;
	unsigned int w;
	unsigned int h;
};

struct game {
	struct board board;
	struct player player;
};


void init_board(struct board *b, unsigned int w, unsigned int h);
tile tile_at(const struct board *b, unsigned int x, unsigned int y);
void set_tile(struct board *b, int x, int y, tile t);
void destroy_board(struct board *b);

#define FRICTION 0.2 
#define SPEED 0.05
#define ACCEL 0.01
void init_player(struct player *p);

void game_step(struct game *g);

#endif
