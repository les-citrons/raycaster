
#define _GNU_SOURCE
#include <math.h>
#include "render.h"

SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;
SDL_Texture *g_wintex = NULL;

int screen_width = 700;
int screen_height = 700;

int render_distance = 90;
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
	copy_vect(&posx, &pos);
	copy_vect(&posy, &pos);

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

	// We choose the closest point of intersection 
	// between x-axis and y-axis facing walls
	if ((distx < disty && distx != 0) || disty == 0)
	{
		copy_vect(&pos, &posx);
		t = tx;
		dist = distx;
	} else {
		copy_vect(&pos, &posy);
		t = ty;
		dist = disty;
	}

	struct ray_view result = {pos, t, dist};
	return result;
}

void draw_view(struct game *g) {
	if (debug_view) {draw_debug(g); return;}

	begin_render();
	struct player *p = &g->player;

	int view_height = screen_width / fov * 100;
	for (int i = 0; i < screen_width; i++) {
		// Cast one ray per column of pixels
		double angle = p->angle - (fov / 2) + (i * (fov / screen_width)); 

		struct ray_view cast = cast_ray(p->pos, angle, &g->board);

		if (cast.hit_tile != 0) {
			// multiply by cosine for very poor fisheye correction
			double dist = sqrt(cast.dist) * cos(d2r(angle - p->angle)); 

			int height = view_height / dist;
			int brightness = 255 / fmax(1, log(dist * 0.2 + 2));
			SDL_Rect col_rect = {i + 1, screen_height/2 - height/2, 1, height};
			SDL_SetRenderDrawColor(g_renderer, brightness, brightness, brightness, brightness);
			SDL_RenderDrawRect(g_renderer, &col_rect);
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

	for (int i = 0; i < screen_width; i++) {
		// Cast one ray per column of pixels
		double angle = p->angle - (fov / 2) + (i * (fov / screen_width)); 

		SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
		struct ray_view cast = cast_ray(p->pos, angle, &g->board);
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
