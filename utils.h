#ifndef _utils_h
#define _utils_h

#include <ncurses.h>
#include <time.h>

void custom_sleep(WINDOW *win, int ms);
int msbetween(struct timespec *time1, struct timespec *time2);
void setdroptime(struct timespec *droptime);
void swapints(int *a, int *b);

#endif
