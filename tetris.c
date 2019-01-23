#include "tetris.h"
#include "utils.h"

#include <stdlib.h>
#include <assert.h>

/* 4 (x, y) coords for each cell in 7 tetrominos in 4 rotations */
int TETRO_COORDS[7][4][4][2] = {
	{{{0, 1}, {1, 1}, {2, 1}, {3, 1}}, /* I */
	 {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
	 {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
	 {{1, 0}, {1, 1}, {1, 2}, {1, 3}}},
	{{{0, 0}, {0, 1}, {1, 1}, {2, 1}}, /* J */
	 {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
	 {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
	 {{1, 0}, {1, 1}, {0, 2}, {1, 2}}},
	{{{2, 0}, {0, 1}, {1, 1}, {2, 1}}, /* L */
	 {{1, 0}, {1, 1}, {1, 2}, {2, 2}}, 
	 {{0, 1}, {1, 1}, {2, 1}, {0, 2}},
	 {{0, 0}, {1, 0}, {1, 1}, {1, 2}}},
	{{{1, 0}, {2, 0}, {1, 1}, {2, 1}}, /* O */
	 {{1, 0}, {2, 0}, {1, 1}, {2, 1}}, 
	 {{1, 0}, {2, 0}, {1, 1}, {2, 1}}, 
	 {{1, 0}, {2, 0}, {1, 1}, {2, 1}}}, 
	{{{1, 0}, {2, 0}, {0, 1}, {1, 1}}, /* S */
	 {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
	 {{1, 1}, {2, 1}, {0, 2}, {1, 2}},
	 {{0, 0}, {0, 1}, {1, 1}, {1, 2}}},
	{{{1, 0}, {0, 1}, {1, 1}, {2, 1}}, /* T */
	 {{1, 0}, {1, 1}, {2, 1}, {1, 2}},
	 {{0, 1}, {1, 1}, {2, 1}, {1, 2}},
	 {{1, 0}, {0, 1}, {1, 1}, {1, 2}}},
	{{{0, 0}, {1, 0}, {1, 1}, {2, 1}}, /* Z */
	 {{2, 0}, {1, 1}, {2, 1}, {1, 2}},
	 {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
	 {{1, 0}, {0, 1}, {1, 1}, {0, 2}}}
};

int inside_board(int x, int y) {
	return x >= 0 && y >= 0 && x < BOARDW && y < BOARDH;
}

int legal_move(int **board, struct tetromino *tetro, int dx, int dy, int dr) {
	int i, x, y;
	for (i = 0; i < 4; i++) {
		x = tetro->x + TETRO_COORDS[tetro->id][(tetro->r + dr) % 4][i][0] + dx;
		y = tetro->y + TETRO_COORDS[tetro->id][(tetro->r + dr) % 4][i][1] + dy;
		if (!inside_board(x, y) || board[y][x] != TETRO_NONE) {
			return 0;
		}
	}
	return 1;
}

void place_tetro(struct tetromino *tetro, int x, int y, int r) {
	tetro->x = x;
	tetro->y = y;
	tetro->r = r;
}

void move_tetro(struct tetromino *tetro, int dx, int dy, int dr) {
	place_tetro(tetro, tetro->x + dx, tetro->y + dy, (tetro->r + dr) % 4);
}

void get_wallkick_offsets(struct tetromino *tetro, int dr, int **dx, int **dy) {
	/* https://tetris.wiki/SRS#Wall_Kicks (y coordinates inverted) */
	static int dx01[5] = {0, -1, -1, 0, -1}, dy01[5] = {0, 0 ,-1, +2, +2};
	static int dx03[5] = {0, +1, +1, 0, +1}, dy03[5] = {0, 0, -1, +2, +2};
	static int dx10[5] = {0, +1, +1, 0, +1}, dy10[5] = {0, 0, +1, -2, -2};
	static int dx12[5] = {0, +1, +1, 0, +1}, dy12[5] = {0, 0, +1, -2, -2};
	static int dx21[5] = {0, -1, -1, 0, -1}, dy21[5] = {0, 0, -1, +2, +2};
	static int dx23[5] = {0, +1, +1, 0, +1}, dy23[5] = {0 ,0, -1, +2, +2};
	static int dx30[5] = {0, -1, -1, 0, -1}, dy30[5] = {0, 0, +1, -2, -2};
	static int dx32[5] = {0, -1, -1, 0, -1}, dy32[5] = {0, 0, +1, -2, -2};
	static int dx01i[5] = {0, -2, +1, -2, +1}, dy01i[5] = {0, 0, 0, +1, -2};
	static int dx03i[5] = {0, -1, +2, -1, +2}, dy03i[5] = {0, 0, 0, -2, +1};
	static int dx10i[5] = {0, +2, -1, +2, -1}, dy10i[5] = {0, 0, 0, -1, +2};
	static int dx12i[5] = {0, -1, +2, -1, +2}, dy12i[5] = {0, 0, 0, -2, +1};
	static int dx21i[5] = {0, +1, -2, +1, -2}, dy21i[5] = {0, 0, 0, +2, -1};
	static int dx23i[5] = {0, +2, -1, +2, -1}, dy23i[5] = {0, 0, 0, -1, +2};
	static int dx30i[5] = {0, -1, +2, -1, +2}, dy30i[5] = {0, 0, 0, -2, +1};
	static int dx32i[5] = {0, -2, +1, -2, +1}, dy32i[5] = {0, 0, 0, +1, -2};

	if (tetro->id == TETRO_I) {
		if (tetro->r == 0 && dr == 1)
			*dx = dx01i, *dy = dy01i;
		else if (tetro->r == 0 && dr == 3)
			*dx = dx03i, *dy = dy03i;
		else if (tetro->r == 1 && dr == 1)
			*dx = dx12i, *dy = dy12i;
		else if (tetro->r == 1 && dr == 3)
			*dx = dx10i, *dy = dy10i;
		else if (tetro->r == 2 && dr == 1)
			*dx = dx23i, *dy = dy23i;
		else if (tetro->r == 2 && dr == 3)
			*dx = dx21i, *dy = dy21i;
		else if (tetro->r == 3 && dr == 1)
			*dx = dx30i, *dy = dy30i;
		else if (tetro->r == 3 && dr == 3)
			*dx = dx32i, *dy = dy32i;
		else
			assert(0);
	} else {
		if (tetro->r == 0 && dr == 1)
			*dx = dx01, *dy = dy01;
		else if (tetro->r == 0 && dr == 3)
			*dx = dx03, *dy = dy03;
		else if (tetro->r == 1 && dr == 1)
			*dx = dx12, *dy = dy12;
		else if (tetro->r == 1 && dr == 3)
			*dx = dx10, *dy = dy10;
		else if (tetro->r == 2 && dr == 1)
			*dx = dx23, *dy = dy23;
		else if (tetro->r == 2 && dr == 3)
			*dx = dx21, *dy = dy21;
		else if (tetro->r == 3 && dr == 1)
			*dx = dx30, *dy = dy30;
		else if (tetro->r == 3 && dr == 3)
			*dx = dx32, *dy = dy32;
		else
			assert(0);
	}
}

int try_translate_tetro(int **board, struct tetromino *tetro, int dx, int dy) {
	if (legal_move(board, tetro, dx, dy, 0)) {
		move_tetro(tetro, dx, dy, 0);
		return 1;
	} else {
		return 0;
	}
}

int try_rotate_tetro(int **board, struct tetromino *tetro, int dr) {
	int i, *dx, *dy;
	struct tetromino tetro_org;
	if (dr % 4 == 2) {
		tetro_org = *tetro;
		/* try rotating once twice in either direction */
		for (dr = 1; dr <= 3; dr += 2) { /* try rotating in both directions */
			for (i = 0; i < 2; i++) {
				if (!try_rotate_tetro(board, tetro, dr)) {
					*tetro = tetro_org;
					break;
				}
			}
			if (i == 2)
				return 1;
		}
		return 0;
	} else {
		get_wallkick_offsets(tetro, dr, &dx, &dy);
		for (i = 0; i < 5; i++) {
			if (legal_move(board, tetro, dx[i], dy[i], dr)) {
				move_tetro(tetro, dx[i], dy[i], dr);
				return 1;
			}
		}
		return 0;
	}
}

void spawn_tetro(int id, struct tetromino *tetro) {
	tetro->id = id;
	place_tetro(tetro, 3, 0, 0);
}

void drop_tetromino(int **board, struct tetromino *tetro) {
	while (legal_move(board, tetro, 0, +1, 0))
		move_tetro(tetro, 0, +1, 0);
}

int above_visible_board(struct tetromino *tetro) {
	/* the last y coordinate is always the lowest y coordinate */
	return tetro->y + TETRO_COORDS[tetro->id][tetro->r][3][1] < 2;
}

void lock_tetromino(int **board, struct tetromino *tetro) {
	int i, x, y;
	for (i = 0; i < 4; i++) {
		x = tetro->x + TETRO_COORDS[tetro->id][tetro->r][i][0];
		y = tetro->y + TETRO_COORDS[tetro->id][tetro->r][i][1];
		board[y][x] = tetro->id;
	}
}

int line_is_full(int *line) {
	int x;
	for (x = 0; x < BOARDW; x++) {
		if (line[x] == TETRO_NONE)
			return 0;
	}
	return 1;
}

void clear_line(int **board, int y) {
	int x;
	for (; y > 0; y--)
		for (x = 0; x < BOARDW; x++)
			board[y][x] = board[y-1][x];
	for (x = 0; x < BOARDW; x++)
		board[0][x] = TETRO_NONE;
}

int clear_lines(int **board, int y1) {
	int y, linescleared;
	linescleared = 0;
	for (y = y1; y < y1 + 4 && y < BOARDH; y++) {
		if (line_is_full(board[y])) {
			clear_line(board, y);
			linescleared++;
		}
	}
	return linescleared;
}

void update_queue(struct tetromino_queue *queue) {
	int i1, i2;
	/* advances queue and randomizes one tetromino in next bag */
	i1 = (queue->idx + 7) % 14;
	i2 = i1 + rand() % (7 - queue->idx % 7);
	swapints(&queue->ids[i1], &queue->ids[i2]);
	queue->idx = (queue->idx + 1) % 14;
}

void shuffle(int *arr, int n) {
	int i;
	for (i = 0; i < n; i++) {
		swapints(&arr[i], &arr[i + rand() % (n - i)]);
	}
}

void init_game(struct tetris *game, int seed) {
	int x, y, id;
	game->board = malloc(BOARDH * sizeof(int *));
	for (y = 0; y < BOARDH; y++) {
		game->board[y] = malloc(BOARDW * sizeof(int));
		for (x = 0; x < BOARDW; x++) 
			game->board[y][x] = TETRO_NONE;
	}
	for (id = 0; id < 7; id++) {
		game->queue.ids[id + 0] = id;
		game->queue.ids[id + 7] = id;
	}
	srand(seed);
	shuffle(game->queue.ids, 7);
	game->queue.idx = 0;
	game->holdid = TETRO_NONE;
	game->canhold = 1;
}

void init_stats(struct statistics *stats) {
	stats->linescleared = 0;
	stats->mselapsed = 0;
	stats->tetrosplaced = 0;
}
