#ifndef PSTREE_H
#define PSTREE_H

#include <stdbool.h>
#include <stddef.h>

struct ps;
struct process;

typedef struct pstree pstree;

pstree *pstree_new(struct ps *);
void pstree_get(pstree *, size_t, struct process **, size_t *indent);

void pstree_free(pstree *);

#endif
