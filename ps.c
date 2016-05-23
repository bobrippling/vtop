#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "ps.h"

#include "mem.h"
#include "proc.h"
#include "io.h"

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

static struct process *ps_alloc_proc(ps *ps)
{
	if(ps->nprocs + 1 > ps->nalloc){
		ps->nalloc = ps->nprocs + 1;
		ps->procs = xrealloc(ps->procs, ps->nalloc * sizeof(*ps->procs));
	}
	struct process *p = &ps->procs[ps->nalloc - 1];
	memset(p, 0, sizeof(*p));
	return p;
}

static void ps_alloc_proc_confirm(ps *ps)
{
	ps->nprocs++;
}

static const char *ps_new_proc_1(ps *ps, pid_t pid)
{
	struct process *p = ps_alloc_proc(ps);

	const char *err = process_init_read(p, pid);

	if(!err)
		ps_alloc_proc_confirm(ps);

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
	static const char *ps_cmd = "ps -e -o pid,ppid,uid,gid,state,nice,tty,command";
	size_t ps_n;
	const char *err;
	char **ps_list = io_readcmd(ps_cmd, &ps_n, 1, &err);
	size_t i;

	if(!ps_list)
		return err;

	for(i = 0; i < ps_n; i++){
		pid_t pid, ppid;
		uid_t uid;
		gid_t gid;
		char stat[8];
		int nice;
		char tty[16];
		int nr;

		int n = sscanf(
				ps_list[i],
				" %d %d %d %d"
				" %7s %d %15s %n",
				&pid, &ppid, &uid, &gid,
				stat, &nice, tty, &nr);

		if(n != 7)
			continue;

		struct process *p;
		size_t proc_idx;
		if((p = ps_get_pid(ps, pid, &proc_idx))){
			currents[proc_idx] = true;
		}else{
			p = ps_alloc_proc(ps);
			ps_alloc_proc_confirm(ps);
		}

		p->pid = pid;
		p->ppid = ppid;
		p->uid = uid;
		p->gid = gid;
		p->state = stat[0];
		p->nice = nice;
		free(p->tty), p->tty = xstrdup(tty);

		char *arg = ps_list[i] + nr;
		argv_from_nulterminated(&p->argv, arg, strlen(ps_list[i]) - nr);
	}


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
