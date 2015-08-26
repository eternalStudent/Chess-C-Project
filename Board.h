#include "PossibleMove.h"

#ifndef BOARD_H
#define BOARD_H

#define Board_WHITE_PAWN   'm'
#define Board_WHITE_BISHOP 'b'
#define Board_WHITE_KNIGHT 'n'
#define Board_WHITE_ROOK   'r'
#define Board_WHITE_QUEEN  'q'
#define Board_WHITE_KING   'k'

#define Board_BLACK_PAWN   'M'
#define Board_BLACK_BISHOP 'B'
#define Board_BLACK_KNIGHT 'N'
#define Board_BLACK_ROOK   'R'
#define Board_BLACK_QUEEN  'Q'
#define Board_BLACK_KING   'K'
#define Board_EMPTY        ' '
#define Board_SIZE         8
#define BLACK 0
#define WHITE 1


typedef struct Board{
	char matrix[8][8];
	int kingX[2];
	int kingY[2];
	int hasKingEverMoved[2];
	int hasRookEverMoved[2][2];
} Board;


Board* Board_new();

int Board_isFurthestRowForPlayer (int player, int y);

void Board_init      (Board* board);

int pieceIsRook(Board* board, int x, int y);

void Board_clear     (Board* board);

void Board_copy      (Board* dest, Board* src);

void Board_setPiece  (Board* board, int x, int y, char piece);

char Board_getPiece  (Board* board, int x, int y);

char Board_removePiece (Board* board, int x, int y);

int  Board_isEmpty   (Board* board, int x, int y);

int Board_getColor(Board* board, int x, int y);

int Board_evalMovesByPiece(Board* board, int x, int y, int player);

int Board_getUpperBoundMoves(Board* board, int player);

void Board_updateKingPosition (Board* board, int x, int y);

int  Board_isInRange (int x, int y);

void Board_update    (Board* board, struct PossibleMove* move);

Board* Board_getPossibleBoard(Board* board, struct PossibleMove* move);

int  Board_evalPiece (Board* board, int x, int y, int color);

int  Board_getScore  (Board* board, int color);

int Board_isInCheck(Board* board, int player);

struct LinkedList* Board_getPossibleMovesOfPiece(Board* board, int x, int y);

struct LinkedList* Board_getPossibleMoves(Board* board, int player);

void Board_print     (Board* board);

void Board_free      (Board* board);

#endif