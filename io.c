#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "io.h"

#include "mem.h"

char *io_readall(const char *path)
{
	size_t len = 0;
	return io_readall_nul(path, &len);
}

char *io_readall_nul(const char *path, size_t *const plen)
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
