
#define _GNU_SOURCE
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include "render.h"

SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_Texture *g_wintex = NULL;

SDL_Texture *g_test_tex = NULL;

int screen_width = 1280;
int screen_height = 720;

int render_distance = 50;
double fov = 100;

int debug_view = 0;

void make_wintex();
void report_error();

void begin_render() {
	SDL_SetRenderTarget(g_renderer, g_wintex);
	SDL_RenderClear(g_renderer);
}

void end_render() {
	SDL_SetRenderTarget(g_renderer, NULL);
	
	SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(g_renderer);
	
	SDL_RenderCopy(g_renderer, g_wintex, NULL, NULL);
	SDL_RenderPresent(g_renderer);
}

struct ray_view {
	struct vect hit_point;
	tile hit_tile;
	double dist;
	int facing_x;
	struct vect direction;
};

struct ray_view cast_ray(struct vect pos, double angle, struct board *b) {
	double a = d2r(angle);

	struct vect direction = {0,1};
	rotate_vect(angle, &direction);

	// Calculate ray intersections in the x direction,
	// then calculate ray intersections in the y direction,
	// and then pick which one is shorter.
	tile t, tx, ty;
	tx = ty = 0;

	struct vect posx, posy;
	posy = posx = pos;

	double face_offs_x = copysign(0.500001, direction.x);
	double face_offs_y = copysign(0.500001, direction.y);

	if (direction.x != 0) {
		// Check each potential x-axis facing wall
		for (int i = 0; i < render_distance; i++) {
			double nextx = round(posx.x) + face_offs_x;
			posx.y += tan(a) * (nextx - posx.x);
			posx.x = nextx;
			
			if ((tx = tile_at(b, round(posx.x + face_offs_x), round(posx.y))) != 0)
				break;
		}
	}

	if (direction.y != 0) {
		// Check each potential y-axis facing wall
		for (int i = 0; i < render_distance; i++) {
			double nexty = round(posy.y) + face_offs_y;
			posy.x += cotangent(a) * (nexty - posy.y);
			posy.y = nexty;
			
			if ((ty = tile_at(b, round(posy.x), round(posy.y + face_offs_y))) != 0)
				break;
		}
	}

	// Fast distance comparison
	double dist, distx, disty;
	distx = sqr(posx.x - pos.x) + sqr(posx.y - pos.y);
	disty = sqr(posy.x - pos.x) + sqr(posy.y - pos.y);

	int facing_x;

	// We choose the closest point of intersection 
	// between x-axis and y-axis facing walls
	if ((distx < disty && distx != 0) || disty == 0)
	{
		facing_x = 1;
		pos = posx;
		t = tx;
		dist = sqrt(distx);
	} else {
		facing_x = 0;
		pos = posy;
		t = ty;
		dist = sqrt(disty);
	}

	struct ray_view result = {pos, t, dist, facing_x, direction};
	return result;
}

void draw_view(struct game *g) {
	if (debug_view) {draw_debug(g); return;}

	begin_render();
	struct player *p = &g->player;
	double p_ang = d2r(p->angle);

	// Get the dimensions of the view plane from the fov
	double viewplane_l = tan(d2r(-fov/2));
	double viewplane_r = tan(d2r(fov/2));
	double viewplane_w = viewplane_r - viewplane_l;

	int view_height = screen_width / fov * 84;
	for (int i = 0; i < screen_width; i++) {
		// Cast one ray per column of pixels
		// Cast rays for equidistant points on the view plane
		double angle = p_ang + atan(viewplane_w/screen_width*i - viewplane_w/2);

		struct ray_view cast = cast_ray(p->pos, r2d(angle), &g->board);

		if (cast.hit_tile != 0) {
			double dist = cos(angle - p_ang) * cast.dist;

			int height = view_height / dist;
			SDL_Rect screen_rect = {i + 1, screen_height/2 - height/2, 1, height};
			
			// Figure out where the ray hits the texture
			int tex_width, tex_height;
			SDL_QueryTexture(g_test_tex, NULL, NULL, &tex_width, &tex_height);

			double hit_point;
			if (cast.facing_x) {
				hit_point = cast.hit_point.y - (round(cast.hit_point.y) - 0.5);
				if (cast.direction.x < 0)
					hit_point = 1 - hit_point;
			}
			else {
				hit_point = cast.hit_point.x - (round(cast.hit_point.x) - 0.5);
				if (cast.direction.y > 0)
					hit_point = 1 - hit_point;
			}
			int tex_pos = tex_width * hit_point;

			SDL_Rect tex_rect = {tex_pos, 0, 1, tex_height};

			int brightness = 255 / fmax(1, log(dist * 0.2 + 2) + 0.15);
			SDL_SetTextureColorMod(g_test_tex, brightness, brightness, brightness);

			SDL_RenderCopy(g_renderer, g_test_tex, &tex_rect, &screen_rect);

			/*
			SDL_SetRenderDrawColor(g_renderer, brightness, brightness, brightness, brightness);
			SDL_RenderDrawRect(g_renderer, &screen_rect);
			*/
		}
	}

	end_render();
}

void draw_debug(struct game *g) {
	begin_render();
	struct player *p = &g->player;

	int zoom = 64;

	int screenx = p->pos.x * zoom;
	int screeny = p->pos.y * zoom;

	SDL_Rect player_rect = {screenx - (zoom / 2), screeny - (zoom / 2), zoom, zoom};
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(g_renderer, &player_rect);

	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0x00, 0xFF);
	SDL_RenderDrawLine(g_renderer, screenx, screeny, 
			screenx + cos(d2r(p->angle)) * zoom, screeny + sin(d2r(p->angle)) * zoom);

	for (int x = 0; x < g->board.w; x++) {
		for (int y = 0; y < g->board.h; y++) {
			SDL_Rect tile_rect = {x * zoom - (zoom / 2), y * zoom - (zoom / 2), zoom, zoom};
			if (tile_at(&g->board, x, y) != 0) {
				SDL_SetRenderDrawColor(g_renderer, 0x00, 0xFF, 0xFF, 0xFF);
				SDL_RenderFillRect(g_renderer, &tile_rect);
			}
		}
	}

	double p_ang = d2r(p->angle);

	double viewplane_l = tan(d2r(-fov/2));
	double viewplane_r = tan(d2r(fov/2));
	double viewplane_w = viewplane_r - viewplane_l;

	for (int i = 0; i < screen_width; i++) {
		// Cast one ray per column of pixels
		double angle = p_ang + atan(viewplane_w/screen_width*i - viewplane_w/2);

		struct ray_view cast = cast_ray(p->pos, r2d(angle), &g->board);

		SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
		SDL_RenderDrawLine(g_renderer, screenx, screeny, 
				cast.hit_point.x * zoom, cast.hit_point.y * zoom);
	}

	end_render();
}

void init(const char *title) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) report_error();

	g_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                                   screen_width, screen_height, SDL_WINDOW_SHOWN |
									   SDL_WINDOW_RESIZABLE);
	if (g_window == NULL) report_error();

	g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | 
	                                              SDL_RENDERER_PRESENTVSYNC |
	                                              SDL_RENDERER_TARGETTEXTURE);
	if (g_renderer == NULL) report_error();

	int flags = IMG_INIT_PNG;
	if (!(IMG_Init(flags) & flags)) {
		printf("æ: %s\n", IMG_GetError());
		exit(-1);
	}
	
	SDL_Surface *s = IMG_Load("test_tex.png");
	if (s == NULL)
	{
		printf("æ: %s\n", IMG_GetError());
		exit(-1);
	}
	g_test_tex = SDL_CreateTextureFromSurface(g_renderer, s);
	if (g_test_tex == NULL) report_error();

	SDL_FreeSurface(s);

	make_wintex(); 
	
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
}

void make_wintex()
{
	g_wintex = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGBA8888,
	                              SDL_TEXTUREACCESS_TARGET, screen_width, screen_height);
}

void report_error()
{
	fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
	exit(-1);
}

void end() {
	SDL_DestroyWindow(g_window);
	SDL_Quit();
	exit(0);
}
