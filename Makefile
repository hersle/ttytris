CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -lncurses
FILES = tetris.o visuals.o highscore.o replay.o utils.o controls.o

ttytris: $(FILES)

clean:
	$(RM) $(FILES) ttytris
