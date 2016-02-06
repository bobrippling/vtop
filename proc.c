#define _BSD_SOURCE /* minor() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proc.h"

#include "io.h"
#include "mem.h"

static void process_read_argv_from_stat(struct process *p)
{
	char proc_path[64];
	snprintf(proc_path, sizeof(proc_path), "/proc/%lu/stat", (long)p->pid);

	char *stat_line = io_readall(proc_path);

	if(!stat_line)
		return;

	char *start = strchr(stat_line, '(');
	if(!start)
		goto out;

	char *end = strchr(start + 1, ')');
	if(!end)
		goto out;

	struct argv *args = &p->argv;

	argv_clear(args);

	args->argc = 1;
	args->argv = xmalloc(2 * sizeof *args->argv);

	*end = '\0';
	args->argv[0] = xstrdup(start + 1);

out:
	free(stat_line);
}

static void process_read_argv(struct process *p)
{
	char path[64];
	snprintf(path, sizeof path, "/proc/%d/cmdline", p->pid);

	size_t cmdlen = 0;
	char *cmd = io_readall_nul(path, &cmdlen);

	if(cmd && cmdlen){
		argv_from_nulterminated(&p->argv, cmd, cmdlen);
	}else{
		process_read_argv_from_stat(p);
	}

	free(cmd);
}

const char *process_init_read(struct process *p, pid_t pid)
{
	memset(p, 0, sizeof *p);
	p->pid = pid;

	char proc_path[64];
	snprintf(proc_path, sizeof(proc_path), "/proc/%lu/stat", (long)pid);

	char *stat_line = io_readall(proc_path);
	if(!stat_line)
		return "open /proc/.../stat:";

	int i = 0;
	char *start = strrchr(stat_line, ')') + 2;
	int ttyn = -1;

	for(char *iter = strtok(start, " \t"); iter; i++, iter = strtok(NULL, " \t")){
		switch(i){
			/* index numbers are zero-based on the first entry after "(process name)" */
			case 0:
			{
				/* state: convert to PROC_STATE */
				char c;
				if(sscanf(iter, "%c", &c) == 1)
					p->state = c;
				break;
			}

#define INT(n, fmt, x) case n: sscanf(iter, fmt, x); break
				INT(1,  "%d", &p->ppid);
				INT(4,  "%d", &ttyn);
				INT(5,  "%u", &p->pgrp);

#if 0
				INT(11, "%lu", &p->utime);
				INT(12, "%lu", &p->stime);
				INT(13, "%lu", &p->cutime);
				INT(14, "%lu", &p->cstime);
#endif
#undef INT
		}
	}
	free(stat_line), stat_line = NULL;

	if(ttyn != -1){
		char ttybuf[16];
		snprintf(ttybuf, sizeof ttybuf, "pts/%d", minor(ttyn));
		p->tty = xstrdup(ttybuf);
	}

	process_read_argv(p);

	return NULL;
}

void process_free(struct process *p)
{
	free(p->unam);
	free(p->gnam);
	free(p->tty);
	argv_clear(&p->argv);
}

