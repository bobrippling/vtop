#include <stdio.h>

#include "ui.h"

int main(int argc, const char *argv[])
{
	if(argc > 1){
		fprintf(stderr, "Usage: %s\n", argv[0]);
		return 1;
	}

	ui_init();
	int ret = ui_main();
	ui_term();

	return ret;
}
