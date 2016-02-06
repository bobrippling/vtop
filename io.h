#ifndef IO_H
#define IO_H

char *io_readall(const char *path);
char *io_readall_nul(const char *path, size_t *const);

#endif
