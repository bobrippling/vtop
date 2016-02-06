#include <sys/time.h>

#include "ui.h"

#include "nc.h"
#include "ps.h"

#include "config.h"

typedef struct {
	ps *ps;
	time_t last_redraw, last_ps_update;
} ui;

static void handle_input(int ch, ui *ui, int *const ec)
{
}

static void maybe_redraw(ui *ui)
{
}

static void maybe_update_ps(ui *ui)
{
}

static int ui_main_1(ui *ui, int *const ec)
{
	int ch = nc_getch_timeout(CONFIG_KEY_TIMEOUT_SECONDS);

	if(ch != NC_GETCH_TIMEOUT)
		handle_input(ch, ui, ec);

	maybe_update_ps(ui);
	maybe_redraw(ui);
}

int ui_main(void)
{
	ui ui = { .ps = ps_new() };

	ui.last_redraw = ui.last_ps_update = time(NULL);

	while(ui_main_1(&ui, &ec));

	ps_free(ui.ps);

	return ec;
}

void ui_init(void)
{
	nc_init();
}

void ui_term(void)
{
	nc_term();
}
