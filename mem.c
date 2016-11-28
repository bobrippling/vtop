#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <errno.h>

#include "mem.h"

static noreturn
void alloc_die(size_t l)
{
	fprintf(stderr, "couldn't allocate %zu bytes: %s\n",
			l, strerror(errno));
	exit(2);
}

void *xmalloc(size_t l)
{
	void *p = malloc(l);

	if(!p)
		alloc_die(l);

	return p;
}

void *xcalloc(size_t n, size_t sz)
{
	void *p = calloc(n, sz);

	if(!p)
		alloc_die(n * sz);

	return p;
}

void *xrealloc(void *p, size_t l)
{
	void *r = realloc(p, l);

	if(!r)
		alloc_die(l);

	return r;
}

char *xstrdup(const char *s)
{
	size_t l = strlen(s) + 1;
	char *new = xmalloc(l);

	memcpy(new, s, l);

	return new;
}
