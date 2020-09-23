
#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "level.h"

SDL_Window *g_window;
SDL_Renderer *g_renderer;
SDL_Texture *g_wintex;

int screen_width;
int screen_height;

void make_wintex();
void draw_debug(struct game *g);
void draw_view(struct game *g);

void init(const char *title);

void end();

int debug_view;

#endif
