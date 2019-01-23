#include "replay.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void add_to_replay(struct replay *rp, int time, int event) {
	assert(rp->len <= rp->cap);
	if (rp->len == rp->cap) {
		rp->cap += 100;
		rp->times = realloc(rp->times, rp->cap * sizeof(int));
		rp->events = realloc(rp->events, rp->cap * sizeof(int));
	}
	rp->times[rp->len] = time;
	rp->events[rp->len] = event;
	rp->len++;
}

char *get_replay_path() {
	static char timefmt[] = "%Y%m%d%H%M%S";
	int len;
	char *homepath, *replaypath, timestr[strlen("yyyymmddhhmmss")+1];
	time_t rawtime;
	homepath = getenv("HOME");
	len = strlen(homepath) + strlen("/.ttytris_replay_yyyymmddhhmmss");
	replaypath = malloc((len + 1) * sizeof(char)); 
	rawtime = time(NULL);
	strftime(timestr, strlen("yyyymmddhhmmss")+1, timefmt, localtime(&rawtime));
	sprintf(replaypath, "%s/.ttytris_replay_%s", homepath, timestr);
	return replaypath;
}

void read_replay(char *path, struct replay *rp) {
	int i;
	FILE *rpfile;
	rpfile = fopen(path, "r");

	/* TODO: use add_to_replay() and remove length from replay files? */
	fscanf(rpfile, "%d\n%d\n", &rp->seed, &rp->len);
	rp->times = malloc(rp->len * sizeof(int));
	rp->events = malloc(rp->len * sizeof(int));
	for (i = 0; i < rp->len; i++) {
		fscanf(rpfile, "%d %d\n", &rp->times[i], &rp->events[i]);
	}
	fclose(rpfile);
}

void save_replay(struct replay *rp) {
	int i;
	char *path;
	FILE *rpfile;
	path = get_replay_path();
	rpfile = fopen(path, "w");

	fprintf(rpfile, "%d\n%d\n", rp->seed, rp->len);
	for (i = 0; i < rp->len; i++) {
		fprintf(rpfile, "%d %d\n", rp->times[i], rp->events[i]);
	}

	free(path);
	fclose(rpfile);
}
