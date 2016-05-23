#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "ps.h"

#include "mem.h"
#include "proc.h"

struct ps
{
	struct process *procs; /* TODO: sorted */
	size_t nprocs, nalloc;

	bool use_fallback;
};

ps *ps_new()
{
	ps *ps = xnew(struct ps);

	ps->procs = NULL;
	ps->nprocs = 0;
	ps->nalloc = 0;

	DIR *proc = opendir("/proc");
	ps->use_fallback = !proc;
	if(proc)
		closedir(proc);

	return ps;
}

void ps_free(ps *ps)
{
	for(size_t i = 0; i < ps->nprocs; i++)
		process_free(&ps->procs[i]);

	free(ps->procs);

	free(ps);
}

struct process *ps_get_index(ps *ps, size_t i)
{
	if(i >= ps->nprocs)
		return NULL;

	return &ps->procs[i];
}

static const char *ps_new_proc_1(ps *ps, pid_t pid)
{
	if(ps->nprocs + 1 > ps->nalloc){
		ps->nalloc = ps->nprocs + 1;
		ps->procs = xrealloc(ps->procs, ps->nalloc * sizeof(*ps->procs));
	}

	struct process *p = &ps->procs[ps->nalloc - 1];
	const char *err = process_init_read(p, pid);

	if(!err)
		ps->nprocs++;

	return err;
}

static const char *ps_update_proc_1(ps *ps, pid_t pid)
{
	return NULL;
}

static void trim_processes(ps *ps, bool *const currents, size_t orig_nprocs)
{
	size_t current_idx = 0;

	for(size_t i = 0; i < orig_nprocs; ){
		if(currents[current_idx++]){
			i++;
			continue;
		}

		process_free(&ps->procs[i]);

		for(size_t j = i; j < ps->nprocs - 1; j++)
			ps->procs[j] = ps->procs[j + 1];

		ps->nprocs--;
		orig_nprocs--;
	}
}

static const char *ps_update_proc(ps *ps, bool *const currents)
{
	DIR *d = opendir("/proc");
	if(!d)
		return "can't open procfs";

	const char *ret = NULL;
	struct dirent *ent;
	while((errno = 0, ent = readdir(d))){
		char *end;
		long lpid = strtol(ent->d_name, &end, 0);

		if(*end)
			continue;

		pid_t pid = (pid_t)lpid;
		size_t idx;

		if(ps_get_pid(ps, pid, &idx)){
			currents[idx] = true;

			ret = ps_update_proc_1(ps, pid);
		}else{
			ret = ps_new_proc_1(ps, pid);
		}

		if(ret)
			goto ret;
	}

	if(errno){
		ret = "readdir:";
	}

ret:;
	int save = errno;
	closedir(d);

	errno = save;
	return ret;
}

static const char *ps_update_fallback(ps *ps, bool *const currents)
{
	return NULL;
}

const char *ps_update(ps *ps)
{
	const size_t orig_nprocs = ps->nprocs;
	bool *const currents = xmalloc(orig_nprocs * sizeof(*currents));
	memset(currents, 0, orig_nprocs * sizeof(*currents));

	const char *err;
	if(ps->use_fallback)
		err = ps_update_fallback(ps, currents);
	else
		err = ps_update_proc(ps, currents);

	trim_processes(ps, currents, orig_nprocs);
	free(currents);

	return err;
}

struct process *ps_get_pid(ps *ps, pid_t pid, size_t *const idx)
{
	for(size_t i = 0; i < ps->nprocs; i++){
		if(ps->procs[i].pid == pid){
			if(idx)
				*idx = i;
			return &ps->procs[i];
		}
	}

	return NULL;
}
