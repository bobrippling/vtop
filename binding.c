#include "binding.h"
#include "config.h"

static void clamp_pos_top(ui_pos *const pos, const point *const frame)
{
	pos->top = clamp(pos->top, pos->y - frame->y + 1 + STATUS_LINES, pos->y);
}

void action_quit(
		ps *ps,
		const point *frame,
		union binding_data const *data,
		struct configurable_state *state)
{
	state->exit_code = data->i;
}

void action_cursor_move(
		ps *ps,
		const point *frame,
		union binding_data const *data,
		struct configurable_state *state)
{
	state->pos.y += data->dir.y;

	if(state->pos.y < 0)
		state->pos.y = 0;

	clamp_pos_top(&state->pos, frame);
}

void action_cursor_goto(
		ps *ps,
		const point *frame,
		union binding_data const *data,
		struct configurable_state *state)
{
	state->pos.y = data->dir.y <= 0 ? 0 : ps_count(ps) - 1;
	if(state->pos.y < 0)
		state->pos.y = 0;

	clamp_pos_top(&state->pos, frame);
}

void action_page_move(
		ps *ps,
		const point *frame,
		union binding_data const *data,
		struct configurable_state *state)
{
	state->pos.top += data->dir.top * (frame->y - STATUS_LINES) / 2;

	if(state->pos.top < 0)
		state->pos.top = 0;

	if(state->pos.y - state->pos.top >= frame->y - STATUS_LINES)
		state->pos.y = state->pos.top + frame->y - STATUS_LINES - 1;
	else if(state->pos.y < state->pos.top)
		state->pos.y = state->pos.top;
}

void action_page_scroll(
		ps *ps,
		const point *frame,
		union binding_data const *data,
		struct configurable_state *state)
{
	state->pos.top += data->dir.top;

	if(state->pos.top < 0)
		state->pos.top = 0;

	state->pos.y = clamp(state->pos.y, state->pos.top, state->pos.top + frame->y - 1 - STATUS_LINES);
}

void action_cursor_page(
		ps *ps,
		const point *frame,
		union binding_data const *data,
		struct configurable_state *state)
{
	state->pos.y = state->pos.top;

	switch(data->dir.y){
		case -1:
			break;
		case 0:
			state->pos.y += (frame->y - STATUS_LINES) / 2;
			break;
		case +1:
			state->pos.y += frame->y - STATUS_LINES - 1;
			break;
	}
}

void action_freeze(
		ps *ps,
		const point *frame,
		union binding_data const *data,
		struct configurable_state *state)
{
	state->frozen ^= 1;
}
