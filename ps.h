#ifndef PS_H
#define PS_H

#include <stddef.h>

typedef struct ps ps;

ps *ps_new(void);
void ps_free(ps *);

void ps_update(ps *);

struct process *ps_process_get(ps *, size_t);

#endif
