#ifndef NC_H
#define NC_H

#include <stdarg.h>
#include "point.h"

void nc_init(void);
void nc_term(void);

int nc_getch_timeout(double timeout_secs);
#define NC_GETCH_TIMEOUT -3

void nc_move(point);
void nc_printf(const char *, ...);
void nc_vprintf(const char *, va_list);

#endif
