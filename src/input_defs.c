
#include "input.h"
#include "input_defs.h"

struct input_def i_move_forward = {
	"Move forward",
	SDLK_w,
};

struct input_def i_move_backward = {
	"Move backward",
	SDLK_s,
};

struct input_def i_strafe_left = {
	"Strafe left",
	SDLK_q,
};

struct input_def i_strafe_right = {
	"Strafe right",
	SDLK_e,
};

struct input_def i_look_left = {
	"Look left",
	SDLK_a,
};

struct input_def i_look_right = {
	"Look right",
	SDLK_d,
};

struct input_def i_action = {
	"Perform action",
	SDLK_SPACE,
};

struct input_def i_debug = {
	"",
	SDLK_DELETE
};

struct input_def i_fast_look_mod = {
	"Hold to look faster",
	SDLK_RSHIFT
};

struct input_def *input_defs[] = {
	&i_move_forward,
	&i_move_backward,
	&i_strafe_left,
	&i_strafe_right,
	&i_look_left,
	&i_look_right,
	&i_action,
	&i_debug
};

