#ifndef NC_H
#define NC_H

void nc_init(void);
void nc_term(void);

int nc_getch(double timeout_secs);

#define NC_GETCH_TIMEOUT -3

#endif
