#ifndef _controls_h
#define _controls_h

#include <ncurses.h>

enum event {
	MOVED, MOVEL, MOVER, ROT1, ROT2, ROT3, DROP, HOLD, NEVENTS
};

static const int controls[NEVENTS] = {
	KEY_DOWN, KEY_LEFT, KEY_RIGHT, 'd', 's', 'a', ' ', 'q'
};

int get_event(int ch);

#endif
