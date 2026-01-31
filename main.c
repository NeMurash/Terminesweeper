#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>

#define GRID_W 16
#define GRID_H 16

#define N_MINES 10

enum Glyphs {
	GLYPH_FILL = '#',
	GLYPH_MINE = 'X',
	GLYPH_FLAG = 'F',
	GLYPH_NONE = ' ',    // Hey Jimmy, give me a character with NOTHING.
};

enum MovementKeys {
	MOVE_UP    = 'w',
	MOVE_DOWN  = 's',
	MOVE_LEFT  = 'a',
	MOVE_RIGHT = 'd',
};

enum SpecialKeys {
	KEY_QUIT   = '\033', // The octal representation of an ASCII escape character or whatever (TL;DR: escape key)
	KEY_REVEAL = ' ',    // Nuuthinn?? (space key)
	KEY_FLAG   = 'f',
};

enum CursorDirections {
	CUR_DIR_UP,
	CUR_DIR_DOWN,
	CUR_DIR_LEFT,
	CUR_DIR_RIGHT,
};

struct Cell {
	char glyph;
	bool mine;
	bool revealed;
	bool flagged;
	int  mineNeighbours;
};

void initTerminal();
void initBoard();
void initBombs();
void quitTerminal();
void updateBoard();
void moveCursor(int distance, int direction);
void revealCell(struct Cell *cell);
void flagCell(struct Cell *cell);
void resetCursor();
void processMovement();

struct termios oldTerminalSettings, newTerminalSettings;
struct Cell cells[GRID_H][GRID_W];

char input;

int dx = 0;
int dy = 0;
int tempX = 0;
int tempY = 0;

int main(void) {
	srand(time(0));

	initTerminal();
	initBoard();
	initBombs();

	updateBoard();

	bool gameOver = false;
	while (!gameOver) {
		input = getchar();

		processMovement();

		switch (input) {
			case KEY_REVEAL:
				revealCell(&cells[abs(dy)][abs(dx)]);

				updateBoard();

				break;
			case KEY_FLAG:
				flagCell(&cells[abs(dy)][abs(dx)]);

				updateBoard();

				break;
			case KEY_QUIT:
				gameOver = true;
				break;
			default: break;
		}
	}

	quitTerminal();

	moveCursor(GRID_H + abs(dy), CUR_DIR_DOWN);

	return 0;
}

void initTerminal() {
	tcgetattr(STDIN_FILENO, &oldTerminalSettings);

	newTerminalSettings = oldTerminalSettings;

	newTerminalSettings.c_lflag &= ~(ICANON);
	newTerminalSettings.c_lflag &= ~(ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, &newTerminalSettings);
}

void initBoard() {
	for (int y=0; y<GRID_H; y++)
		for (int x=0; x<GRID_W; x++)
			cells[y][x] = (struct Cell) {
				GLYPH_FILL,
				false,
				false,
				false,
				0,
			};
}

void initBombs() {
	int i = N_MINES;
	while (i >= 0) {
		int x = rand() % (GRID_W + 1);
		int y = rand() % (GRID_H + 1);

		if (!cells[y][x].mine) {
			cells[y][x].mine = true;
			i--;
		}
	}
}

void quitTerminal() {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);
}

void updateBoard() {
	tempX = dx;
	tempY = dy;
	resetCursor();

	for (int y=0; y<GRID_H; y++) {
		for (int x=0; x<GRID_W; x++) {
			struct Cell *currentCell = &cells[y][x];
			printf("%c ", currentCell->glyph);
		}
		printf("\n");
	}
	moveCursor(GRID_H, CUR_DIR_UP);

	// Restore the cursor position
	if  (tempX != 0) {
		moveCursor(abs(tempX) * 2, CUR_DIR_RIGHT);
		dx = tempX;
	}
	if  (tempY != 0) {
		moveCursor(abs(tempY), CUR_DIR_DOWN);
		dy = tempY;
	}
}

void moveCursor(int distance, int direction) {
	switch (direction) {
		case CUR_DIR_UP:
			printf("\033[%dA", distance);
			break;
		case CUR_DIR_DOWN:
			printf("\033[%dB", distance);
			break;
		case CUR_DIR_LEFT:
			printf("\033[%dD", distance);
			break;
		case CUR_DIR_RIGHT:
			printf("\033[%dC", distance);
			break;
	}
}

void revealCell(struct Cell *cell) {
	if (!cell->revealed && !cell->flagged) {
		cell->revealed = true;
		if (cell->mine)
			cell->glyph = GLYPH_MINE;
		else {
			cell->glyph = GLYPH_NONE;
		}
	}
}

void flagCell(struct Cell *cell) {
	if (!cell->revealed) {
		cell->flagged = !cell->flagged;
		if (cell->flagged)
			cell->glyph = GLYPH_FLAG;
		else
			cell->glyph = GLYPH_FILL;
	}
}

void resetCursor() {
	if (dx != 0) {
		moveCursor(abs(dx)*2, CUR_DIR_LEFT);
		dx = 0;
	}
	if (dy != 0) {
		moveCursor(abs(dy), CUR_DIR_UP);
		dy = 0;
	}
}

void processMovement() {
	switch (input) {
		case MOVE_UP:
			moveCursor(1, CUR_DIR_UP);
			dy++;
			if (dy > 0) {
				moveCursor(1, CUR_DIR_DOWN);
				dy--;
			}
			break;
		case MOVE_DOWN:
			moveCursor(1, CUR_DIR_DOWN);
			dy--;
			if (dy < -GRID_H+1) {
				moveCursor(1, CUR_DIR_UP);
				dy++;
			}
			break;
		case MOVE_LEFT:
			moveCursor(2, CUR_DIR_LEFT);
			dx--;
			if (dx < 0) {
				dx++;
			}
			break;
		case MOVE_RIGHT:
			moveCursor(2, CUR_DIR_RIGHT);
			dx++;
			if (dx >= GRID_W) {
				moveCursor(2, CUR_DIR_LEFT);
				dx--;
			}
			break;
		default: break;
	}
}
