#ifndef IO_H
#define IO_H

char *io_readline(const char *path);
char *io_readline_nul(const char *path, size_t *const);

char **io_readcmd(const char *, size_t *, size_t skip, const char **const err);

#endif
