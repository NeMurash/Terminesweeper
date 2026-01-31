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
	GLYPH_NONE = ' ',
};

enum MovementKeys {
	MOVE_UP    = 'w',
	MOVE_DOWN  = 's',
	MOVE_LEFT  = 'a',
	MOVE_RIGHT = 'd',
};

enum SpecialKeys {
	KEY_QUIT   = 'q',
	KEY_REVEAL = ' ',
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
	bool revealed;
	bool mine;
};

void initTerminal();
void initBoard();
void initBombs();
void quitTerminal();
void displayBoard();
void moveCursor(int distance, int direction);
void processMovement();

struct termios oldTerminalSettings, newTerminalSettings;
struct Cell cells[GRID_H][GRID_W];

enum GameStates {
	STATE_CHOOSING,
	STATE_REVEALED,
	STATE_FLAGGED,
	STATE_LOST,
} gameState = STATE_CHOOSING;

char input;

int dx = 0;
int dy = 0;

int main(void) {
	srand(time(0));

	initTerminal();
	initBoard();
	initBombs();

	displayBoard();

	bool gameOver = false;
	while (!gameOver) {
		input = getchar();

		switch (gameState) {
			case STATE_CHOOSING:
				processMovement();

				switch (input) {
					case KEY_REVEAL:
						gameState = STATE_REVEALED;
						break;
					case KEY_FLAG:
						gameState = STATE_FLAGGED;
						break;
					case KEY_QUIT:
						gameOver = true;
						break;
					default: break;
				}
				break;
			case STATE_REVEALED:
				gameState = STATE_CHOOSING;
				break;
			case STATE_FLAGGED:
				gameState = STATE_CHOOSING;
				break;
			case STATE_LOST:
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
	for (int y=0; y<GRID_H; y++) {
		for (int x=0; x<GRID_W; x++) {
			cells[y][x].glyph = GLYPH_FILL;
			cells[y][x].revealed = false;
			cells[y][x].mine     = false;
		}
	}
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

void displayBoard() {
	for (int y=0; y<GRID_H; y++) {
		for (int x=0; x<GRID_W; x++) {
			printf("%c ", cells[y][x].glyph);
		}
		printf("\n");
	}
	moveCursor(GRID_H, CUR_DIR_UP);
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
