#include "binding.h"

void action_quit(
		ps *ps,
		ui_pos *const pos,
		int *const exit_code,
		union binding_data const *data)
{
	*exit_code = data->i;
}

void action_cursor_move(
		ps *ps,
		ui_pos *const pos,
		int *const exit_code,
		union binding_data const *data)
{
	pos->y += data->dir.y;
	pos->top += data->dir.top;

	if(pos->y < 0) pos->y = 0;
	if(pos->top < 0) pos->top = 0;
}
