#ifndef _visuals_h
#define _visuals_h

#include "tetris.h"
#include <ncurses.h>

WINDOW *boardwin, *queuewin, *holdwin, *statwin;
WINDOW *boardbwin, *queuebwin, *holdbwin, *statbwin; /* for drawing borders */

void display_hold(int holdid);
void display_queue(struct tetromino_queue *queue);
void display_stats(struct statistics *stats);
void display_countdown(int ms);
void display_game(struct tetris *game, struct statistics *stats);
void display_highscores();

void init_curses();
void handle_resize();

#endif
