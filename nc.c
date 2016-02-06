#include <ncurses.h>

#include "nc.h"

static void ui_init_color(void)
{
	start_color();
	use_default_colors();

	init_pair(1 + COLOR_BLACK   , COLOR_BLACK  , -1);
	init_pair(1 + COLOR_GREEN   , COLOR_GREEN  , -1);
	init_pair(1 + COLOR_WHITE   , COLOR_WHITE  , -1);
	init_pair(1 + COLOR_RED     , COLOR_RED    , -1);
	init_pair(1 + COLOR_CYAN    , COLOR_CYAN   , -1);
	init_pair(1 + COLOR_MAGENTA , COLOR_MAGENTA, -1);
	init_pair(1 + COLOR_BLUE    , COLOR_BLUE   , -1);
	init_pair(1 + COLOR_YELLOW  , COLOR_YELLOW , -1);
}

void nc_init(void)
{
	static bool done = false;

	if(done){
		refresh();
		return;
	}

	done = true;

	initscr();
	noecho();
	cbreak();
	raw();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);

	if(has_colors())
		ui_init_color();
}

void nc_term(void)
{
	static bool done = false;

	if(done){
		refresh();
		return;
	}

	done = true;

	initscr();
	noecho();
	cbreak();
	raw();
	nonl();
}

int nc_getch_timeout(double timeout)
{
	if(timeout <= 0){
		/* no timeout */
		nocbreak();
	}else{
		halfdelay(timeout * 20);
	}

	int ch = getch();

	if(timeout > 0 && ch == ERR)
		return NC_GETCH_TIMEOUT;

	return ch;
}

