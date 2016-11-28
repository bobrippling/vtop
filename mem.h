#ifndef MEM_H
#define MEM_H

#include <stddef.h>

void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
void *xrealloc(void *, size_t);
char *xstrdup(const char *);

#define xnew(T) (T *)xmalloc(sizeof(T))

#endif
