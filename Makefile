CFLAGS = -O2 -Wall -Wextra
LDFLAGS = -lncurses
SRC = tetris.c visuals.c highscore.c replay.c utils.c controls.c
OBJ = $(SRC:.c=.o)
TARGET = ttytris
INSTALL_DIR = /usr/local/bin

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

install: $(TARGET)
	install -m 755 $< $(INSTALL_DIR)

clean:
	$(RM) $(OBJ) $(TARGET)
