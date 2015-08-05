#include "PieceCounter.h"
#include "Board.h"
#include <ctype.h>

int max[7] = {8, 2, 2, 1, 1, 1, 1};

static int isOnWhite(int x, int y){
	return (x+y)%2;
}

static int pieceToNum(char piece, int x, int y){
	switch(piece){
		case Board_BLACK_PAWN:
		case Board_WHITE_PAWN:   return 0;
		case Board_BLACK_KNIGHT: 
		case Board_WHITE_KNIGHT: return 1;
		case Board_BLACK_ROOK:
		case Board_WHITE_ROOK:   return 2;
		case Board_BLACK_BISHOP:
		case Board_WHITE_BISHOP: return isOnWhite(x,y)? 3: 4;
		case Board_BLACK_QUEEN:
		case Board_WHITE_QUEEN:  return 5;
		case Board_BLACK_KING:
		case Board_WHITE_KING:   return 6;
	}
	return -1; //should never happen
}

static int getColor(char piece){
	return toupper(piece) == piece? BLACK: WHITE;
}

int PieceCounter_isAtMax(int counter[2][7], char piece, int x, int y){
	int num = pieceToNum(piece, x, y);
	int color = getColor(piece);
	return counter[color][num] == max[num];
}

void PieceCounter_update(int counter[2][7], char piece, int amountToAdd, int x, int y){
	if (piece == Board_EMPTY){
		return;
	}
	int num = pieceToNum(piece, x, y);
	int color = getColor(piece);
	counter[color][num] += amountToAdd;
}

void PieceCounter_reset(int counter[2][7]){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 7; j++){
			counter[i][j] = 0;
		}	
	}
}

void PieceCounter_setToMax(int counter[2][7]){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 7; j++){
			counter[i][j] = max[j];
		}	
	}
}

int PieceCounter_kingIsMissing(int counter[2][7]){
	return counter[WHITE][6] == 0 || counter[BLACK][6] == 0;
}