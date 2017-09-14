#ifndef UI_POS_H
#define UI_POS_H

typedef struct ui_pos
{
	int y, top;
} ui_pos;

int clamp(int v, int min, int max);

#endif
