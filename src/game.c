
#include <SDL2/SDL.h>
#include "game.h"
#include "render.h"
#include "event.h"

int game_started = 0;
struct game main_game = {0};

const short the_board[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
};

void start() {
	init("raycast");
	init_board(&main_game.board, 10, 10);
	memcpy(main_game.board.tiles, the_board, 100 * sizeof(short));
	init_player(&main_game.player);

	game_started = 1;
}

void mainloop() {
	SDL_SetRelativeMouseMode(SDL_TRUE);
	while (1) {
		handle_events();
		draw_view(&main_game);
		game_step(&main_game);
	}
}
