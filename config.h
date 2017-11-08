#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_KEY_TIMEOUT_SECONDS 0.5
#define CTRL_AND(c) ((c) & 037)
#define KEY_ESC CTRL_AND('[')

#define STATUS_LINES 1

static const binding bindings[] = {
	{ "q", action_quit },
	{ "j", action_cursor_move, .data.dir.y = +1 },
	{ "k", action_cursor_move, .data.dir.y = -1 },

	{ (char []){ CTRL_AND('e'), '\0' }, action_page_scroll, .data.dir.top = +1 },
	{ (char []){ CTRL_AND('y'), '\0' }, action_page_scroll, .data.dir.top = -1 },

	{ (char []){ CTRL_AND('f'), '\0' }, action_page_move, .data.dir.top = +2 },
	{ (char []){ CTRL_AND('b'), '\0' }, action_page_move, .data.dir.top = -2 },
	{ (char []){ CTRL_AND('d'), '\0' }, action_page_move, .data.dir.top = +1 },
	{ (char []){ CTRL_AND('u'), '\0' }, action_page_move, .data.dir.top = -1 },

	{ "H", action_cursor_page, .data.dir.y = -1 },
	{ "M", action_cursor_page, .data.dir.y =  0 },
	{ "L", action_cursor_page, .data.dir.y = +1 },

	{ "gg", action_cursor_goto, .data.dir.y = -1 },
	{ "G", action_cursor_goto, .data.dir.y = 1 },

	{ 0 },
};

#endif
