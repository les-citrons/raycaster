
#include "input.h"
#include "input_defs.h"
#include <SDL2/SDL.h>

double mouse_sensitivity = 0.2;
int mouse_motion = 0;

int check_key(struct input_def *i) {
	return SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(i->value)];
}
