#ifndef _tetris_h
#define _tetris_h

#define BOARDW 10
#define BOARDH 22

enum tetromino_id {
	TETRO_I, TETRO_J, TETRO_L, TETRO_O, TETRO_S, TETRO_T, TETRO_Z, TETRO_NONE
};

enum play_mode {
	MODE_PLAY, MODE_REPLAY
};

struct tetromino {
	int id;
	int x, y, r;
};

struct tetromino_queue {
	int ids[14];
	int idx;
};

struct tetris {
	int **board;
	struct tetromino falling;
	struct tetromino_queue queue;
	int holdid, canhold;
};

struct statistics {
	int linescleared;
	int mselapsed;
	int tetrosplaced;
};

int TETRO_COORDS[7][4][4][2]; /* 4 (x, y) coords for 4 rotations for 7 tetros */

void place_tetro(struct tetromino *tetro, int x, int y, int r);
void move_tetro(struct tetromino *tetro, int dx, int dy, int dr);
int legal_move(int **board, struct tetromino *tetro, int dx, int dy, int dr);
int try_translate_tetro(int **board, struct tetromino *tetro, int dx, int dy);
int try_rotate_tetro(int **board, struct tetromino *tetro, int dr);

void spawn_tetro(int id, struct tetromino *tetro);
void drop_tetromino(int **board, struct tetromino *tetro);
void lock_tetromino(int **board, struct tetromino *tetro);
int clear_lines(int **board, int y1);

int above_visible_board(struct tetromino *tetro);

void update_queue(struct tetromino_queue *queue);

void init_game(struct tetris *game, int seed);
void init_stats(struct statistics *stats);

#endif
