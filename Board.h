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


Board* Board_new();

int Board_isFurthestRowForPlayer (int player, int y);

void Board_init      (Board* board);

void Board_clear     (Board* board);

void Board_copy      (Board* dest, Board* src);

void Board_setPiece  (Board* board, int x, int y, char piece);

char Board_getPiece  (Board* board, int x, int y);

char Board_remove    (Board* board, int x, int y);

int  Board_isEmpty   (Board* board, int x, int y);

int  Board_isInRange (int x, int y);

int  Board_isPlayable(Board* board, int counter[2][7]);

void Board_update    (Board* board, struct PossibleMove* move);

Board* Board_getPossibleBoard(Board* board, struct PossibleMove* move);

int  Board_evalPiece (Board* board, int x, int y, int color);

int  Board_getScore  (Board* board, int color);

struct LinkedList* Board_getPossibleMoves(Board* board, int player);

void Board_print     (Board* board);

void Board_free      (Board* board);