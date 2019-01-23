#include "highscore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_highscore_path() {
	int len;
	char *homepath, *highscorepath;
	homepath = getenv("HOME");
	len = strlen(homepath) + strlen("/.ttytris_highscores");
	highscorepath = malloc((len + 1) * sizeof(char)); 
	sprintf(highscorepath, "%s/.ttytris_highscores", homepath);
	return highscorepath;
}

int read_highscores(int *highscores) {
	int i;
	char *path;
	FILE *hsfile;
	path = get_highscore_path();
	hsfile = fopen(path, "r");
	if (hsfile == NULL)
		return 0;
	for (i = 0; i < NHIGHSCORES; i++) {
		if (fscanf(hsfile, "%d", &highscores[i]) == EOF)
			break;
	}
	free(path);
	fclose(hsfile);
	return i;
}

void save_highscore(int score) {
	char *path;
	int i, nhighscores, highscores[NHIGHSCORES];
	FILE *hsfile;
	nhighscores = read_highscores(highscores);
	path = get_highscore_path();
	hsfile = fopen(path, "w");
	for (i = 0; i < nhighscores && highscores[i] < score; i++) {
		fprintf(hsfile, "%d\n", highscores[i]);
	}
	if (i < NHIGHSCORES) {
		fprintf(hsfile, "%d\n", score);
		for (; i < NHIGHSCORES - 1 && i < nhighscores; i++) {
			fprintf(hsfile, "%d\n", highscores[i]);
		}
	}
	free(path);
	fclose(hsfile);
}
