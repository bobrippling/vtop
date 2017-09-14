#include "binding.h"

#define STATUS_LINES 1

void action_quit(
		ps *ps,
		ui_pos *const pos,
		const point *frame,
		int *const exit_code,
		union binding_data const *data)
{
	*exit_code = data->i;
}

void action_cursor_move(
		ps *ps,
		ui_pos *const pos,
		const point *frame,
		int *const exit_code,
		union binding_data const *data)
{
	pos->y += data->dir.y;

	if(pos->y < 0)
		pos->y = 0;

	pos->top = clamp(pos->top, pos->y - frame->y + 1 + STATUS_LINES, pos->y);
}

void action_page_move(
		ps *ps,
		ui_pos *const pos,
		const point *frame,
		int *const exit_code,
		union binding_data const *data)
{
	pos->top += data->dir.top * (frame->y - STATUS_LINES) / 2;

	if(pos->top < 0)
		pos->top = 0;

	if(pos->y - pos->top >= frame->y - STATUS_LINES)
		pos->y = pos->top + frame->y - STATUS_LINES - 1;
	else if(pos->y < pos->top)
		pos->y = pos->top;
}

void action_cursor_page(
		ps *ps,
		ui_pos *const pos,
		const point *frame,
		int *const exit_code,
		union binding_data const *data)
{
	pos->y = pos->top;

	switch(data->dir.y){
		case -1:
			break;
		case 0:
			pos->y += (frame->y - STATUS_LINES) / 2;
			break;
		case +1:
			pos->y += frame->y - STATUS_LINES - 1;
			break;
	}
}
