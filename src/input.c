
#include "input.h"
#include "input_defs.h"
#include <SDL2/SDL.h>

int check_key(struct input_def *i) {
	return SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(i->value)];
}
