#include <stdbool.h>
#include <stdlib.h>

#include "argv.h"

#include "mem.h"

void argv_clear(struct argv *args)
{
	for(int i = 0; i < args->argc; i++)
		free(args->argv[i]);
	args->argv = NULL;
	args->argc = 0;
}

void argv_from_nulterminated(struct argv *args, const char *str, size_t len)
{
	argv_clear(args);

	args->argc = 1;
	args->argv = xmalloc(2 * sizeof *args->argv);
	args->argv[0] = xstrdup(str);
	args->argv[1] = NULL;

	return;

	size_t nuls = 0;
	for(size_t i = 0; i < len; i++)
		if(str[i] == '\0')
			nuls++;

	bool last_is_nul = len > 0 && str[len - 1] == '\0';

	args->argc = nuls + (last_is_nul ? 0 : 1);
	args->argv = xmalloc((args->argc + 1) * sizeof *args->argv);

	const char *last = str;
	size_t n = 0;
	for(size_t i = 0; i < len; i++){
		if(str[i] == '\0'){
			args->argv[n++] = xstrdup(last);
			last = str + i + 1;
		}
	}
	args->argv[args->argc] = NULL;
}
