#include "visuals.h"
#include "highscore.h"
#include "tetris.h"
#include "utils.h"

#include <ncurses.h>
#include <string.h>
#include <assert.h>

void display_cell(WINDOW *win, int x, int y, int id, int isghost) {
	chtype ch;
	if (y >= 0) {
		ch = (isghost ? '#' : ' ' | A_REVERSE) | COLOR_PAIR(id + 1);
		mvwaddch(win, y, 2 * x + 0, ch);
		mvwaddch(win, y, 2 * x + 1, ch);
	}
}

void display_board(int **board) {
	int x, y;
	for (y = 2; y < BOARDH; y++)
		for (x = 0; x < BOARDW; x++)
			display_cell(boardwin, x, y - 2, board[y][x], 0);
	wnoutrefresh(boardwin);
}

void display_tetro(WINDOW *win, struct tetromino *tetro, int isghost) {
	int i, x, y;
	for (i = 0; i < 4; i++) {
		x = tetro->x + TETRO_COORDS[tetro->id][tetro->r][i][0];
		y = tetro->y + TETRO_COORDS[tetro->id][tetro->r][i][1];
		if (win == boardwin)
			y -= 2; /* top two lines in board are hidden */
		display_cell(win, x, y, tetro->id, isghost);
	}
	wnoutrefresh(win);
}

void display_queue(struct tetromino_queue *queue) {
	int i;
	struct tetromino tetro;
	place_tetro(&tetro, 0, 0, 0);
	werase(queuewin);
	for (i = 0; i < 5; i++) {
		tetro.id = queue->ids[(queue->idx + i) % 14];
		display_tetro(queuewin, &tetro, 0);
		tetro.y += 4;
	}
	wnoutrefresh(queuewin);
}

void display_hold(int holdid) {
	struct tetromino tetro;
	werase(holdwin);
	if (holdid != TETRO_NONE) {
		tetro.id = holdid;
		place_tetro(&tetro, 0, 0, 0);
		display_tetro(holdwin, &tetro, 0);
	}
	wnoutrefresh(holdwin);
}

void display_ghost(int **board, struct tetromino *tetro) {
	struct tetromino ghost;
	ghost = *tetro;
	drop_tetromino(board, &ghost);
	display_tetro(boardwin, &ghost, 1);
}

void display_stats(struct statistics *stats) {
	int m, s;
	double tetrospersec, linespersec;
	m = stats->mselapsed / 60000;
	s = (stats->mselapsed / 1000) % 60;
	if (stats->mselapsed == 0) {
		tetrospersec = 0.0;
		linespersec = 0.0;
	} else {
		tetrospersec = 1000.0 * stats->tetrosplaced / stats->mselapsed;
		linespersec = 1000.0 * stats->linescleared / stats->mselapsed;
	}
	werase(statwin);
	mvwprintw(statwin, 0, 0, "Lines");
	mvwprintw(statwin, 1, 0, "> %d", 40 - stats->linescleared);
	mvwprintw(statwin, 3, 0, "Time");
	mvwprintw(statwin, 4, 0, "> %02d:%02d", m, s);
	mvwprintw(statwin, 6, 0, "Tetros");
	mvwprintw(statwin, 7, 0, "> %d", stats->tetrosplaced);
	mvwprintw(statwin, 9, 0, "Lines/s");
	mvwprintw(statwin, 10, 0, "> %.3f", linespersec);
	mvwprintw(statwin, 12, 0, "Tetros/s");
	mvwprintw(statwin, 13, 0, "> %.3f", tetrospersec);
	wnoutrefresh(statwin);
}

void display_game(struct tetris *game, struct statistics *stats) {
	display_board(game->board);
	display_ghost(game->board, &game->falling);
	display_tetro(boardwin, &game->falling, 0);
	display_hold(game->holdid);
	display_queue(&game->queue);
	display_stats(stats);
	doupdate();
}

void display_centered_string(WINDOW *win, char *str) {
	int x, y;
	getmaxyx(win, y, x);
	assert(strlen(str) <= (unsigned) x);
	x = (x - strlen(str)) / 2;
	y = (y - 1) / 2;
	werase(win);
	mvwaddstr(win, y, x, str);
	wrefresh(win);
}

void display_countdown(int msinterval) {
	display_centered_string(boardwin, "READY");
	custom_sleep(boardwin, msinterval);
	display_centered_string(boardwin, "SET");
	custom_sleep(boardwin, msinterval);
	display_centered_string(boardwin, "GO!");
	custom_sleep(boardwin, msinterval);
}

void center_windows() {
	int x, y;
	getmaxyx(stdscr, y, x);
	x = (x - 40) / 2;
	y = (y - 22) / 2;
	mvwin(holdbwin, y, x);
	mvwin(holdwin, y + 1, x + 1);
	mvwin(statbwin, y + 5, x);
	mvwin(statwin, y + 6, x + 1);
	mvwin(boardbwin, y, x + 9);
	mvwin(boardwin, y + 1, x + 10);
	mvwin(queuebwin, y, x + 30);
	mvwin(queuewin, y + 1, x + 31);
	wborder(boardbwin, 0, 0, 0, 0, ACS_TTEE, ACS_TTEE, ACS_BTEE, ACS_BTEE);
	wborder(holdbwin, 0, 0, 0, 0, 0, ACS_TTEE, ACS_LTEE, ACS_RTEE);
	wborder(statbwin, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, ACS_BTEE);
	wborder(queuebwin, 0, 0, 0, 0, ACS_TTEE, 0, ACS_BTEE, 0);
	wnoutrefresh(boardbwin);
	wnoutrefresh(holdbwin);
	wnoutrefresh(statbwin);
	wnoutrefresh(queuebwin);
	doupdate();
}

void init_curses() {
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	set_escdelay(0);

	/* TODO: make work with on colors */
	if (has_colors() == TRUE && start_color() == OK && COLOR_PAIRS >= 9) {
		init_pair(TETRO_I + 1, COLOR_CYAN, COLOR_BLACK);
		init_pair(TETRO_J + 1, COLOR_BLUE, COLOR_BLACK);
		init_pair(TETRO_L + 1, COLOR_YELLOW, COLOR_BLACK);
		init_pair(TETRO_O + 1, COLOR_WHITE, COLOR_BLACK);
		init_pair(TETRO_S + 1, COLOR_GREEN, COLOR_BLACK);
		init_pair(TETRO_T + 1, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(TETRO_Z + 1, COLOR_RED, COLOR_BLACK);
		init_pair(TETRO_NONE + 1, COLOR_BLACK, COLOR_BLACK);
	}

	holdbwin = newwin(6, 10, 0, 0);
	holdwin = newwin(4, 8, 1, 1);
	statwin = newwin(15, 8, 6, 1);
	statbwin = newwin(17, 10, 5, 0);
	boardwin = newwin(20, 20, 1, 10);
	boardbwin = newwin(22, 22, 0, 9);
	queuewin = newwin(20, 8, 1, 31);
	queuebwin = newwin(22, 10, 0, 30);

	keypad(boardwin, TRUE);
	center_windows();
}

void display_highscores() {
	int i, nhighscores, highscores[NHIGHSCORES], m, s, ms;

	nhighscores = read_highscores(highscores);
	werase(boardwin);
	mvwaddstr(boardwin, 1, 10 - strlen("High scores") / 2, "High scores");
	for (i = 0; i < nhighscores; i++) {
		if (highscores[i] >= 100 * 60 * 1000)
			highscores[i] = 100 * 60 * 1000 - 1;
		m = highscores[i] / 60000;
		s = (highscores[i] / 1000) % 60;
		ms = highscores[i] % 1000;
		mvwprintw(boardwin, 3 + i, 4, "%2d. %02d:%02d:%03d", i + 1, m, s, ms);
	}
	for (; i < NHIGHSCORES; i++) {
		mvwprintw(boardwin, 3 + i, 4, "%2d. --:--:---", i + 1);
	}
	mvwaddstr(boardwin, 16, 5, "ESC to exit");
	mvwaddstr(boardwin, 17, 4, "ENTER to play");
	mvwaddstr(boardwin, 18, 2, "S to save replay");
	wrefresh(boardwin);
}

void handle_resize() {
	endwin();
	clear();
	refresh();
	center_windows();
}
