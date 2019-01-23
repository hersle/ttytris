#include "visuals.h"
#include "tetris.h"
#include "replay.h"
#include "highscore.h"
#include "utils.h"
#include "controls.h"

#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>
#include <time.h>

/* 
 * tetris game conforming to the Tetris Guideline and the SRS 
 * https://tetris.wiki/Tetris_Guideline
 * https://tetris.wiki/SRS
 */

void play(struct replay *rp) {
	struct tetris game;
	int ch, event;
	struct statistics stats;
	struct timespec time1, time2;
	struct timespec droptime;
	int gamewon, gamelost;
	int play_mode;
	int seed;
	int locktetro;
	int replayidx;

	if (rp == NULL) {
		play_mode = MODE_PLAY;
		seed = time(NULL);
		rp = malloc(sizeof(struct replay));
		rp->len = 0;
		rp->cap = 0;
		rp->times = rp->events = NULL;
		rp->seed = seed;
	} else {
		play_mode = MODE_REPLAY;
		seed = rp->seed;
		replayidx = 0;
		ch = 0;
	}

	init_game(&game, seed);
	init_stats(&stats);
	gamewon = 0;
	gamelost = 0;

	display_hold(game.holdid);
	display_queue(&game.queue);
	display_stats(&stats);
	display_countdown(500);

	spawn_tetro(game.queue.ids[game.queue.idx], &game.falling);
	update_queue(&game.queue);
	setdroptime(&droptime);

	display_game(&game, &stats);

	while (!gamewon && !gamelost) {
		clock_gettime(CLOCK_MONOTONIC, &time1);
		if (play_mode == MODE_PLAY) {
			wtimeout(boardwin, msbetween(&time1, &droptime));
			ch = wgetch(boardwin);
			event = get_event(ch);
		} else {
			custom_sleep(boardwin, rp->times[replayidx]);
			event = rp->events[replayidx];
			if (replayidx >= rp->len)
				ch = 27; /* end game */
			replayidx++;
		}
		clock_gettime(CLOCK_MONOTONIC, &time2);
		stats.mselapsed += msbetween(&time1, &time2);

		locktetro = 0;
		if (event == MOVED) {
			if (try_translate_tetro(game.board, &game.falling, 0, +1)) {
				setdroptime(&droptime);
			} else {
				locktetro = 1;
			}
		} else if (event == MOVEL) {
			if (try_translate_tetro(game.board, &game.falling, -1, 0)) {
				if (!legal_move(game.board, &game.falling, 0, +1, 0))
					setdroptime(&droptime);
			}
		} else if (event == MOVER) {
			if (try_translate_tetro(game.board, &game.falling, +1, 0)) {
				if (!legal_move(game.board, &game.falling, 0, +1, 0))
					setdroptime(&droptime);
			}
		} else if (event == ROT1 || event == ROT2 || event == ROT3) {
			if (try_rotate_tetro(game.board, &game.falling, event - ROT1 + 1)) {
				if (!legal_move(game.board, &game.falling, 0, +1, 0))
					setdroptime(&droptime);
			}
		} else if (event == DROP) {
			drop_tetromino(game.board, &game.falling);
			locktetro = 1;
		} else if (event == HOLD) {
			if (game.holdid == TETRO_NONE) {
				game.holdid = game.falling.id;
				spawn_tetro(game.queue.ids[game.queue.idx], &game.falling);
				update_queue(&game.queue);
				game.canhold = 0;
			} else if (game.canhold) {
				swapints(&game.holdid, &game.falling.id);
				spawn_tetro(game.falling.id, &game.falling);
				game.canhold = 0;
			}
		} else if (ch == 'r') {
			return play(NULL);
		} else if (ch == 27) {
			gamelost = 1;
		}

		if (locktetro) {
			lock_tetromino(game.board, &game.falling);
			if (above_visible_board(&game.falling)) {
				gamelost = 1;
			} else {
				stats.linescleared += clear_lines(game.board, game.falling.y);
				stats.tetrosplaced++;
				game.canhold = 1;
				spawn_tetro(game.queue.ids[game.queue.idx], &game.falling);
				update_queue(&game.queue);
				setdroptime(&droptime);
			}
		}

		if (play_mode == MODE_PLAY && event != -1) {
			add_to_replay(rp, msbetween(&time1, &time2), event);
		}

		if (!legal_move(game.board, &game.falling, 0, 0, 0))
			gamelost = 1;
		else if (stats.linescleared >= 40)
			gamewon = 1;

		display_game(&game, &stats);
	}

	doupdate();

	if (gamewon) 
		save_highscore(stats.mselapsed);

	display_highscores();

	wtimeout(boardwin, -1);
	do {
		ch = wgetch(boardwin);
	} while (ch != '\n' && ch != 27 && ch != 's');

	if (ch == '\n')
		play(NULL);
	if (ch == 's')
		save_replay(rp);
	if (ch == 27)
		return;
}

int main(int argc, char *argv[]) {
	struct replay *rp;

	if (argc >= 2) {
		rp = malloc(sizeof(struct replay));
		read_replay(argv[1], rp);
	} else {
		rp = NULL;
	}

	init_curses();
	signal(SIGWINCH, handle_resize);
	play(rp);
	endwin();
	return 0;
}

