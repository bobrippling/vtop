#ifndef BINDING_H
#define BINDING_H

#include "ui_pos.h"
#include "ps.h"

union binding_data
{
	int i;
	ui_pos dir;
};

typedef void binding_action(
		ps *,
		ui_pos *const,
		int *const exit_code /*-1*/,
		union binding_data const *);

typedef struct binding
{
	const char *keys;
	binding_action *action;

	union binding_data data;
} binding;

binding_action action_quit;
binding_action action_cursor_move;

#endif
