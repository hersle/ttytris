#include "controls.h"

#include <ncurses.h>

int get_event(int ch) {
	int event;
	if (ch == ERR)
		return MOVED;
	for (event = 0; event < NEVENTS; event++)
		if (controls[event] == ch)
			return event;
	return -1;
}
