#ifndef NC_H
#define NC_H

#include <stdarg.h>
#include "point.h"
#include "compiler.h"

void nc_init(void);
void nc_term(void);

int nc_getch_timeout(double timeout_secs);
#define NC_GETCH_TIMEOUT -3

void nc_move(point);
point nc_get_screensz(void);

void nc_printf(const char *, ...) attr_printf(1, 2);
void nc_vprintf(const char *, va_list);

void nc_clrtoeol(void);
void nc_clrtobot(void);

#endif
