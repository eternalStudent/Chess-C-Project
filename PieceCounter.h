#include "Board.c"

int PieceCounter_isAtMax(int counter[2][7], char piece, int x, int y);

void PieceCounter_update(int counter[2][7], char piece, int amountToAdd, int x, int y);

void PieceCounter_reset(int counter[2][7]);

void PieceCounter_setToMax(int counter[2][7]);