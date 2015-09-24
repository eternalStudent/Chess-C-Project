#ifndef POSSIBLEMOVE_H
#define POSSIBLEMOVE_H

typedef struct Board{
	char matrix[8][8];
	int kingX[2];
	int kingY[2];
	int hasKingEverMoved[2];
	int hasRookEverMoved[2][2];
} Board;


typedef struct PossibleMove{
	int fromX, fromY, toX, toY;
	char promotion;
	Board* board;
} PossibleMove;

PossibleMove* PossibleMove_new(int, int, int, int, char, Board*);

int PossibleMove_equals(PossibleMove* this, PossibleMove* other);

void PossibleMove_print(PossibleMove*);

PossibleMove* PossibleMove_clone (PossibleMove* move);

void PossibleMove_free(void*);

#endif