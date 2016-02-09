#ifndef PS_H
#define PS_H

#include <stddef.h>
#include <sys/types.h>

typedef struct ps ps;

ps *ps_new(void);
void ps_free(ps *);

const char *ps_update(ps *);

struct process *ps_get_index(ps *, size_t);
struct process *ps_get_pid(ps *ps, pid_t pid, size_t *const idx);

#endif
