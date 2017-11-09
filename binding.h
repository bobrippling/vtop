#ifndef BINDING_H
#define BINDING_H

#include <stdbool.h>

#include "ui_pos.h"
#include "point.h"
#include "ps.h"

union binding_data
{
	int i;
	ui_pos dir;
};

struct configurable_state
{
	ui_pos pos;
	int exit_code; /* if >= 0, exit */
	bool frozen;
};

typedef void binding_action(
		ps *,
		point const *frame,
		union binding_data const *,
		struct configurable_state *state);

typedef struct binding
{
	const char *keys;
	binding_action *action;

	union binding_data data;
} binding;

binding_action action_quit;
binding_action action_cursor_move;
binding_action action_page_move;
binding_action action_cursor_page;
binding_action action_page_scroll;
binding_action action_cursor_goto;
binding_action action_freeze;

#endif
