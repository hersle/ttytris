#ifndef _replay_h
#define _replay_h

struct replay {
	int seed;
	int *times, *events;
	int len, cap;
};

void add_to_replay(struct replay *rp, int time, int event);
void read_replay(char *path, struct replay *rp);
void save_replay(struct replay *rp);

#endif
