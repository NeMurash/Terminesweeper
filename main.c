#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#define GRID_W 16
#define GRID_H 16

enum Glyphs {
	GLYPH_FILL = '#',
	GLYPH_MINE = 'X',
	GLYPH_FLAG = 'F',
	GLYPH_NONE = ' ',
};

struct Cell {
	char glyph;
};

void initTerminal();
void quitTerminal();

struct termios oldTerminal, newTerminal;

int main(void) {
	initTerminal();

	struct Cell cells[GRID_H][GRID_W];
	for (int y=0; y<GRID_H; y++) {
		for (int x=0; x<GRID_W; x++) {
			cells[y][x].glyph = GLYPH_FILL;
			printf("%c ", cells[y][x].glyph);
		}
		printf("\n");
	}

	char c;
	int dx = 0;
	int dy = 0;
	while ((c = getchar()) != 'q') {
		switch (c) {
			case 'w':
				printf("\033[1A");
				dy++;
				break;
			case 's':
				printf("\033[1B");
				dy--;
				break;
			case 'd':
				printf("\033[2C");
				dx++;
				break;
			case 'a':
				printf("\033[2D");
				dx--;
				break;
			default: break;
		}
	}

	quitTerminal();

	return 0;
}

void initTerminal() {
	tcgetattr(STDIN_FILENO, &oldTerminal);

	newTerminal = oldTerminal;

	newTerminal.c_lflag &= ~(ICANON);
	newTerminal.c_lflag &= ~(ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newTerminal);
}

void quitTerminal() {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminal);
}
