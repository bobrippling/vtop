#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_KEY_TIMEOUT_SECONDS 0.5
#define CTRL_AND(c) ((c) & 037)

static const binding bindings[] = {
	{ "q", action_quit },
	{ "j", action_cursor_move, .data.dir.top = +1 },
	{ "k", action_cursor_move, .data.dir.top = -1 },
	{ 0 },
};

#endif
