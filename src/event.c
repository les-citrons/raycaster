
#include <SDL2/SDL.h>
#include "event.h"
#include "render.h"

void handle_events() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				end(); break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
					screen_width = e.window.data1;
					screen_height = e.window.data2;
					SDL_SetWindowSize(g_window, screen_width, screen_height);

					SDL_DestroyTexture(g_wintex);
					make_wintex();
				}
				break;
		}
	}
}
