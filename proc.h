#ifndef PROC_H
#define PROC_H

#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>

#include "argv.h"

struct process
{
	pid_t pid, ppid;

	uid_t uid;
	gid_t gid;
	gid_t pgrp;

	char *unam, *gnam;

	char *tty;

	struct argv argv;

	char state;

	signed char nice;
};

const char *process_init_read(struct process *, pid_t);
void process_free(struct process *); /* doesn't release argument, just contents */

/* returns false on death */
bool process_try_update(struct process *, const char **err);

#define process_is_kernel(p) ((p)->ppid == 0 || (p)->ppid == 2)

#endif
