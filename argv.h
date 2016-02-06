#ifndef ARGV_H
#define ARGV_H

struct argv
{
	char **argv;
	int argc;
};

#define argv_init(p) memset(p, 0, sizeof(*p))
void argv_clear(struct argv *);

#endif
