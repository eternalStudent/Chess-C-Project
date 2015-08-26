#ifndef POSSIBLEMOVE_H
#define POSSIBLEMOVE_H

#include "Board.h"

struct PossibleMove{
	int fromX, fromY, toX, toY;
	char promotion;
	Board* board;
};

struct PossibleMove* PossibleMove_new(int, int, int, int, char, Board*);

int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other);

void PossibleMove_print(struct PossibleMove*);

struct PossibleMove* PossibleMove_clone (struct PossibleMove* move);

void PossibleMove_free(void*);

#endif