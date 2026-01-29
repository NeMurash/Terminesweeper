#include <stdio.h>
#include <stdlib.h>
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

enum Directions {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
};

struct Cell {
	char glyph;
	bool bomb;
};

void initTerminal();
void resetTerminal();
void moveCursor(int distance, int direction);
void processMovement();

struct termios oldTerminal, newTerminal;
struct Cell cells[GRID_H][GRID_W];

char c;
int dx = 0;
int dy = 0;

int main(void) {
	initTerminal();

	for (int y=0; y<GRID_H; y++) {
		for (int x=0; x<GRID_W; x++) {
			cells[y][x].glyph = GLYPH_FILL;
			cells[y][x].bomb = false;
			printf("%c ", cells[y][x].glyph);
		}
		printf("\n");
	}
	moveCursor(GRID_H, DIR_UP);

	while (c != 'q') {
		c = getchar();
		processMovement();
	}

	resetTerminal();

	moveCursor(GRID_H + abs(dy), DIR_DOWN);

	return 0;
}

void initTerminal() {
	tcgetattr(STDIN_FILENO, &oldTerminal);

	newTerminal = oldTerminal;

	newTerminal.c_lflag &= ~(ICANON);
	newTerminal.c_lflag &= ~(ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newTerminal);
}

void resetTerminal() {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminal);
}

void moveCursor(int distance, int direction) {
	switch (direction) {
		case DIR_UP:
			printf("\033[%dA", distance);
			break;
		case DIR_DOWN:
			printf("\033[%dB", distance);
			break;
		case DIR_LEFT:
			printf("\033[%dD", distance);
			break;
		case DIR_RIGHT:
			printf("\033[%dC", distance);
			break;
	}
}

void processMovement() {
	switch (c) {
		case 'w':
			moveCursor(1, DIR_UP);
			dy++;
			if (dy > 0) {
				moveCursor(1, DIR_DOWN);
				dy--;
			}
			break;
		case 's':
			moveCursor(1, DIR_DOWN);
			dy--;
			if (dy < -GRID_H+1) {
				moveCursor(1, DIR_UP);
				dy++;
			}
			break;
		case 'a':
			moveCursor(2, DIR_LEFT);
			dx--;
			break;
		case 'd':
			moveCursor(2, DIR_RIGHT);
			dx++;
			if (dx >= GRID_W) {
				moveCursor(2, DIR_LEFT);
				dx--;
			}
			break;
		default: break;
	}
}
