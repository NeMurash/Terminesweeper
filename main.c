#include <stdio.h>

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

int main(void) {
	struct Cell cells[GRID_H][GRID_W];
	for (int y=0; y<GRID_H; y++) {
		for (int x=0; x<GRID_W; x++) {
			cells[y][x].glyph = GLYPH_FILL;
			printf("%c ", cells[y][x].glyph);
		}
		printf("\n");
	}

	return 0;
}
