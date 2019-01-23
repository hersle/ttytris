#include "utils.h"

#include <ncurses.h>
#include <time.h>

int msbetween(struct timespec *past, struct timespec *future) {
	return (future->tv_sec - past->tv_sec) * 1000 + 
	       (future->tv_nsec - past->tv_nsec) / 1000000;
}

void setdroptime(struct timespec *droptime) {
	clock_gettime(CLOCK_MONOTONIC, droptime);
	droptime->tv_nsec += 750000000;
}

void custom_sleep(WINDOW *win, int ms) {
	struct timespec time1, time2;
	while (ms > 0) {
		clock_gettime(CLOCK_MONOTONIC, &time1);
		wtimeout(win, ms);
		wgetch(win);
		clock_gettime(CLOCK_MONOTONIC, &time2);
		ms -= msbetween(&time1, &time2);
	}
}

void swapints(int *a, int *b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}
