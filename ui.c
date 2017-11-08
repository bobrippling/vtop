#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <math.h>
#include <sys/time.h>

#include "ui.h"

#include "mem.h"
#include "nc.h"
#include "ps.h"
#include "proc.h"
#include "ui_pos.h"
#include "binding.h"
#include "pstree.h"

#include "config.h"

typedef struct {
	ps *ps;
	time_t last_redraw, last_ps_update;
	const char *last_ps_err;
	char *unhandled_input;
	int exit_code; /* if >= 0, exit */
	ui_pos pos;
	point frame;
} ui;

static void append_ch(char **const unhandled_input, int ch)
{
	size_t l;
	if(*unhandled_input)
		l = strlen(*unhandled_input);
	else
		l = 0;

	l++;
	*unhandled_input = xrealloc(*unhandled_input, l + 1);
	(*unhandled_input)[l - 1] = ch;
	(*unhandled_input)[l] = '\0';
}

static void handle_input(int ch, ui *ui)
{
	append_ch(&ui->unhandled_input, ch);

	for(binding const *i = bindings; i->keys; i++){
		if(!strcmp(i->keys, ui->unhandled_input)){
			free(ui->unhandled_input);
			ui->unhandled_input = NULL;

			i->action(ui->ps, &ui->pos, &ui->frame, &ui->exit_code, &i->data);
			break;
		}
	}
}

static void maybe_redraw(ui *ui)
{
	pstree *tree = pstree_new(ui->ps);

	ui->frame = nc_get_screensz();
	int y = 0;
	const unsigned maxpidspace = log10(ps_maxpid(ui->ps)) + 1;

	for(size_t i = ui->pos.top; y < ui->frame.y - STATUS_LINES; i++){
		size_t indent;
		struct process *p;
		pstree_get(tree, i, &p, &indent);
		if(!p)
			break;

		nc_move((point){ .y = y++ });

		nc_printf("%*ld ", maxpidspace, p->pid);
		for(; indent > 0; indent--)
			nc_printf("  ");

		char **argv = p->argv.argv;
		nc_printf("%s", argv ? argv[0] : "?");

		nc_clrtoeol();
	}

	pstree_free(tree);
	tree = NULL;

	nc_move((point){ .y = y });
	nc_clrtobot();

	const char *msg = ui->last_ps_err;
	if(msg){
		nc_move((point){ .y = ui->frame.y - 1 });
		nc_printf("%s", msg);
	}

	nc_move((point){ .y = ui->pos.y - ui->pos.top });
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
