#ifndef PS_H
#define PS_H

#include <stddef.h>
#include <sys/types.h>
#include "compiler.h"

typedef struct ps ps;

attr_wur
ps *ps_new(void);
void ps_free(ps *);

attr_wur
const char *ps_update(ps *);

attr_wur
struct process *ps_get_index(ps *, size_t);
struct process *ps_get_pid(ps *ps, pid_t pid, size_t *const idx);

size_t ps_count(ps *);

pid_t ps_maxpid(ps *);

#endif
