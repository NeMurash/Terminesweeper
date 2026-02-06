#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>

#define COLOUR_WHITE   "\033[37m"
#define COLOUR_GRAY    "\033[90m"
#define COLOUR_RED     "\033[31m"
#define COLOUR_BRED    "\033[91m"
#define COLOUR_BBLUE   "\033[94m"
#define COLOUR_BLUE    "\033[34m"
#define COLOUR_GREEN   "\033[32m"
#define COLOUR_CYAN    "\033[36m"
#define COLOUR_BCYAN   "\033[96m"
#define COLOUR_YELLOW  "\033[33m"
#define COLOUR_MAGENTA "\033[35m"
#define COLOUR_CLEAR   "\033[39;49m"

#define GRID_W 16
#define GRID_H 16

#define N_MINES 20

#define ENABLE_AUDIO   false
#define ENABLE_COLOURS true

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
};

void initTerminal();
void initBoard();
void initMines();
void quitTerminal();
void updateBoard();
void moveCursor(int distance, int direction);
void revealCell(int x, int y);
void areaRevealCell(int x, int y);
void flagCell(struct Cell *cell);
void resetCursor();
void processMovement();
bool isAMine(int x, int y);
bool isAFlag(int x, int y);
bool isValidCell(int x, int y);
bool gameWon();
int countCellMineNeighbours(int x, int y);
int countCellFlaggedNeighbours(int x, int y);

struct termios oldTerminalSettings, newTerminalSettings;
struct Cell cells[GRID_H][GRID_W];

char input;

int dx = 0;
int dy = 0;
int tempX = 0;
int tempY = 0;

bool gameOver = false;
bool gameLost = true;

int main(void) {
	srand(time(0));

	initTerminal();
	initBoard();
	initMines();

	updateBoard();

	while (!gameOver) {
		input = getchar();

		processMovement();

		switch (input) {
			case KEY_REVEAL:
				areaRevealCell(dx, dy);
				revealCell(dx, dy);

				updateBoard();

				if (ENABLE_AUDIO)
					system("aplay -q sounds/blip3.wav &");

				if (gameWon()) {
					for (int i=0; i<GRID_H; i++) {
						for (int j=0; j<GRID_W; j++) {
							if (cells[i][j].mine)
								cells[i][j].glyph = GLYPH_MINE;
						}
					}
					updateBoard();
					gameOver = true;
				}

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

	printf("\r");

	if (!gameLost) {
		if (ENABLE_AUDIO)
			system("aplay -q sounds/win.wav &");
		printf("YOU WON!!!! BOOYAH!!!!\n");
	}
	else {
		if (ENABLE_AUDIO)
			system("aplay -q sounds/boom.wav &");
		printf("You lost :((\n");
	}

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
			};
}

void initMines() {
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
			if (ENABLE_COLOURS) {
				switch (currentCell->glyph) {
					case GLYPH_MINE:
						printf(COLOUR_RED"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case GLYPH_FLAG:
						printf(COLOUR_RED"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '1':
						printf(COLOUR_CYAN"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '2':
						printf(COLOUR_GREEN"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '3':
						printf(COLOUR_BRED"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '4':
						printf(COLOUR_BLUE"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '5':
						printf(COLOUR_YELLOW"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '6':
						printf(COLOUR_BCYAN"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '7':
						printf(COLOUR_GRAY"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					case '8':
						printf(COLOUR_WHITE"%c "COLOUR_CLEAR, currentCell->glyph);
						break;
					default:
						printf("%c "COLOUR_CLEAR, currentCell->glyph);
						break;
				}
			}
			else {
				printf("%c ", currentCell->glyph);
			}
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

// Oh my God bruh
void revealCell(int x, int y) {
	if (isValidCell(x, y)) {
		struct Cell *cell = &cells[abs(y)][abs(x)];
		if (!cell->revealed && !cell->flagged) {
			cell->revealed = true;
			if (cell->mine) {
				cell->glyph = GLYPH_MINE;
				for (int i=0; i<GRID_H; i++) {
					for (int j=0; j<GRID_W; j++) {
						if (cells[i][j].mine)
							cells[i][j].glyph = GLYPH_MINE;
					}
				}
				updateBoard();
				gameOver = true;
			}
			else {
				int mineNbors = countCellMineNeighbours(x, y);
				if (mineNbors == 0) {
					cell->glyph = GLYPH_NONE;
					for (int ty=y-1; ty<y+2; ty++) {
						for (int tx=x-1; tx<x+2; tx++) {
							revealCell(tx, ty);
						}
					}
				}
				else 
					cell->glyph = (char) ('0' + mineNbors);
			}
		}
	}
}

void areaRevealCell(int x, int y) {
	if (isValidCell(x, y)) {
		struct Cell *cell = &cells[abs(y)][abs(x)];
		if (cell->revealed) {
			int flagNbors = countCellFlaggedNeighbours(x, y);
			if (flagNbors >= atoi(&cell->glyph)) {
				for (int ty=y-1; ty<y+2; ty++) {
					for (int tx=x-1; tx<x+2; tx++) {
						revealCell(tx, ty);
					}
				}
			}
		}
	}
}

void flagCell(struct Cell *cell) {
	if (!cell->revealed) {
		cell->flagged = !cell->flagged;
		if (ENABLE_AUDIO)
			system("aplay -q sounds/blip2.wav &");
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

bool isAMine(int x, int y) {
	if (isValidCell(x, y) && cells[abs(y)][abs(x)].mine)
		return true;
	return false;
}

bool isAFlag(int x, int y) {
	if (isValidCell(x, y) && cells[abs(y)][abs(x)].flagged)
		return true;
	return false;
}

bool isValidCell(int x, int y) {
	if ((0 >= y && y > -GRID_H)&&(0 <= x && x < GRID_W))
		return true;
	return false;
}

bool gameWon() {
	for (int i=0; i<GRID_H; i++) {
		for (int j=0; j<GRID_W; j++) {
			struct Cell *cell = &cells[i][j];
			if (!cell->mine && !cell->revealed)
				return false;
		}
	}
	gameLost = false;
	return true;
}

int countCellMineNeighbours(int cellX, int cellY) {
	int nbors = 0;

	for (int y=cellY-1; y<cellY+2; y++) {
		for (int x=cellX-1; x<cellX+2; x++) {
			if (y == x) continue;
			if (isAMine(x, y)) nbors++;
		}
	}

	return nbors;
}

int countCellFlaggedNeighbours(int cellX, int cellY) {
	int nbors = 0;

	for (int y=cellY-1; y<cellY+2; y++) {
		for (int x=cellX-1; x<cellX+2; x++) {
			if (y == x) continue;
			if (isAFlag(x, y)) nbors++;
		}
	}

	return nbors;
}
