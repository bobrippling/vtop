#include <time.h>
#include <sys/time.h>

#include "ui.h"

#include "nc.h"
#include "ps.h"
#include "proc.h"

#include "config.h"

typedef struct {
	ps *ps;
	time_t last_redraw, last_ps_update;
	const char *last_ps_err;
	int exit_code; /* if >= 0, exit */
} ui;

static void handle_input(int ch, ui *ui)
{
	if(ch == 'q'){
		ui->exit_code = 0;
	}
}

static void maybe_redraw(ui *ui)
{
	point max = nc_get_screensz();
	int y = 0;

	for(size_t i = 0; y < max.y - 1; i++){
		struct process *p = ps_get_index(ui->ps, i);
		if(!p)
			break;
		if(process_is_kernel(p))
			continue;

		nc_move((point){ .y = y++ });

		char **argv = p->argv.argv;

		nc_printf("%ld %s", p->pid, argv ? argv[0] : "?");
	}

	nc_move((point){ .y = y });
	nc_clrtobot();

	const char *msg = ui->last_ps_err;
	if(msg){
		nc_move((point){ .y = max.y - 1 });
		nc_printf("%s", msg);
	}
}

static void maybe_update_ps(ui *ui)
{
	time_t now = time(NULL);

	if(ui->last_ps_update + 1 < now){
		ui->last_ps_update = now;

		ui->last_ps_err = ps_update(ui->ps);
	}
}

static void ui_main_1(ui *ui)
{
	maybe_update_ps(ui);
	maybe_redraw(ui);

	int ch = nc_getch_timeout(CONFIG_KEY_TIMEOUT_SECONDS);
	if(ch != NC_GETCH_TIMEOUT)
		handle_input(ch, ui);
}

int ui_main(void)
{
	ui ui = { .ps = ps_new(), .exit_code = -1 };

	ui.last_redraw = ui.last_ps_update = time(NULL) - 2;

	for(;;){
		ui_main_1(&ui);
		if(ui.exit_code >= 0)
			break;
	}

	ps_free(ui.ps);

	return ui.exit_code;
}

void ui_init(void)
{
	nc_init();
}

void ui_term(void)
{
	nc_term();
}
