#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "io.h"

#include "mem.h"

char *io_readline(const char *path)
{
	size_t len = 0;
	return io_readline_nul(path, &len);
}

char *io_readline_nul(const char *path, size_t *const plen)
{
	FILE *f = fopen(path, "r");

	if(!f)
		return NULL;

	size_t n = 0;
	char *buf = NULL;

	ssize_t read_count = getline(&buf, &n, f);
	const int read_err = errno;

	fclose(f);

	if(read_count < 0){
		errno = read_err;
		free(buf);
		return NULL;
	}

	*plen = read_count;

	return buf;
}

static char **io_readall(FILE *f, size_t *const plen, size_t skip, const char **const err)
{
	char **list = NULL;
	size_t n = 0;

	char *line = NULL;
	ssize_t read_count = 0;
	size_t alloc_count = 0;

	while((read_count = getline(&line, &alloc_count, f)) > 0){
		if(skip > 0){
			skip--;
			continue;
		}

		list = xrealloc(list, ++n * sizeof *list);

		list[n-1] = xstrdup(line);
	}

	if(ferror(f))
		*err = "read()";

	*plen = n;
	return list;
}

char **io_readcmd(const char *cmd, size_t *const nlines, size_t skip, const char **const err)
{
	FILE *pipe = popen(cmd, "r");

	char **ret = io_readall(pipe, nlines, skip, err);

	pclose(pipe);

	return ret;
}
