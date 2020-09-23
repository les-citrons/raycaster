
#ifndef RAYCAST_INPUT_H
#define RAYCAST_INPUT_H

#include <SDL2/SDL.h>

// A definition for an assignable in-game input
struct input_def {
	const char *name;
	SDL_Keycode value;
};

int check_key(struct input_def *i); 

#endif
