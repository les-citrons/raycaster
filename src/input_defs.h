
#ifndef INPUT_DEFS_H
#define INPUT_DEFS_H

#include "input.h"

struct input_def i_move_forward;
struct input_def i_move_backward;
struct input_def i_strafe_left;
struct input_def i_strafe_right;
struct input_def i_look_left;
struct input_def i_look_right;
struct input_def i_action;
struct input_def i_fast_look_mod;
struct input_def i_unlock_mouse;
struct input_def i_debug;

struct input_def *input_defs[10];

#endif
