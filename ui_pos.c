#include "ui_pos.h"

#ifndef MAX
#  define MAX(a, b) ((a) > (b) ? (a) : (b))
#  define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

int clamp(int v, int min, int max)
{
	return MAX(min, MIN(max, v));
}
