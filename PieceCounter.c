#include "PieceCounter.h"
#include "Board.h"
#include <ctype.h>


/* 
 * Describes the maximum number of pieces allowed on the board for each player, in this order:
 * Pawn, Knight, Rook, Bishop on white tile, Bishop on black tile, QueenKing.
 */

int max[7] = {8, 2, 2, 1, 1, 1, 1};


/*
 * @return: 1 if (x,y) is a white tile, 0 otherwise
 */
static int isOnWhite(int x, int y){
	return (x+y)%2;
}

/*
 * Auxilary function for assigning a number for each type of piece, 
 * so the appropriate counter could be updated when necessary.
 * @params: (piece) - the piece to be evaluated
 *          (x, y) - the location of the piece
 * @return: the index of the appropriate counter for this piece 
 */

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

/*
 * @return: BLACK(0) if (piece) belongs to the black player, 
 *			WHITE(1) if it belongs to the white player
 */
static int getColor(char piece){
	return toupper(piece) == piece? BLACK: WHITE;
}

/*
 * Checks whether a counter for a specific piece is at its maximum legal value.
 * @params: (counter) - the counter array to be checked
 *			(piece) - the piece type to be checked
 *		    (x, y) - the specifiec piece's location on the board
 * @return: 1 if the relevant counter is at its max legal value, 0 otherwise
 */
int PieceCounter_isAtMax(int counter[2][7], char piece, int x, int y){
	int num = pieceToNum(piece, x, y);
	int color = getColor(piece);
	return counter[color][num] == max[num];
}

/*
 * Updates the relevant counter.
 * @params: (counter) - the counter array to be updated
 *			(piece) - the piece type to be updated
 *		    (x, y) - the piece's location on the board
 *			(amountToAdd) - the counter will be updated by this much.
 */
void PieceCounter_update(int counter[2][7], char piece, int amountToAdd, int x, int y){
	if (piece == Board_EMPTY){
		return;
	}
	int num = pieceToNum(piece, x, y);
	int color = getColor(piece);
	counter[color][num] += amountToAdd;
}

/*
 * Resets an entire counter array to 0.
 * @params: (counter) - the counter array to be reset.
 */
void PieceCounter_reset(int counter[2][7]){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 7; j++){
			counter[i][j] = 0;
		}	
	}
}

/*
 * Sets an entire counter array to maximum legal values for each piece type.
 * @params: (counter) - the counter array to be set.
 */
void PieceCounter_setToMax(int counter[2][7]){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 7; j++){
			counter[i][j] = max[j];
		}	
	}
}

/*
 * Checks whether one of the kings is missing on the board counted by (counter).
 * @params: (counter) - the counter array to be checked.
 * 
 * @return: 1 if one the kings is missing, 0 otherwise
 */
int PieceCounter_kingIsMissing(int counter[2][7]){
	return counter[WHITE][6] == 0 || counter[BLACK][6] == 0;
}

/*
 * Copies the contents of one piece counter structure to another.
 * @params: (dest) - the counter array the data will be copied to.
 *			(src) - the counter array the data will be copied from.
 */
void PieceCounter_copy(int dest[2][7], int src[2][7]){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 7; j++){
			dest[i][j] = src[i][j];
		}
	}
}