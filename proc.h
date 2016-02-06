#ifndef PROC_H
#define PROC_H

#include <stdbool.h>
#include <sys/time.h>

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

  enum state
	{
		state_run,
		state_sleep,
		state_disk,
		state_stopped,
		state_zombie,
		state_dead,
		state_trace,
		state_other,
		state_COUNT
	} state;

  signed char nice;
};

bool process_init_read(struct process *, pid_t);
void process_free(struct process *);

/* returns false on death */
bool process_try_update(struct process *);

#endif
