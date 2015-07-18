#include "PossibleMoveList.c"

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

struct counterSet{
	int whitePawnCounter;
	int blackPawnCounter;
	int whiteBishopOnBlackTileCounter;
	int whiteBishopOnWhiteTileCounter;
	int blackBishopOnBlackTileCounter;
	int blackBishopOnWhiteTileCounter;
	int whiteRookCounter;
	int blackRookCounter;
	int whiteKnightCounter;
	int blackKnightCounter;
	int whiteQueenCounter;
	int blackQueenCounter;
	int whiteKingCounter;
	int blackKingCounter;
};


char** Board_new();

void Board_init      (char** board);

struct counterSet* newCounterSet();

int canPieceBeAdded(struct counterSet* pieceCounters, char piece, int x, int y);

void updatePieceCounter(struct counterSet* pieceCounters, char piece, int amountToAdd, int x, int y);

void resetCounters(struct counterSet* pieceCounters);

void Board_clear     (char** board);

void Board_copy      (char** dest, char** src);

void Board_setPiece  (char** board, int x, int y, char piece);

char Board_getPiece  (char** board, int x, int y);

char Board_remove    (char** board, int x, int y);

int  Board_isEmpty   (char** board, int x, int y);

int  Board_isInRange (int x, int y);

int  Board_isPlayable(char** board, struct counterSet* pieceCounters);

void Board_update    (char** board, struct PossibleMove* move);

char** Board_getPossibleBoard(char** board, struct PossibleMove* move);

int  Board_evalPiece (char** board, int x, int y, int color);

int  Board_getScore  (char** board, int color);

struct LinkedList* Board_getPossibleMoves(char** board, int player, int check[2]);

void Board_print     (char** board);

void Board_free      (char** board);