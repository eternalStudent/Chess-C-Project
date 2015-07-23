#include <stdio.h>
#include <ctype.h>
#include "Board.h"

/*
 * Creates a new board structure.
 *
 * @return:  NULL if an allocation error occurred, 
 *           a pointer to a new board otherwise
 */
Board* Board_new(){
	Board* board = (Board*)calloc(1, sizeof(Board));
	Board_init(board);
	return board;
}

/*
 * Populates the board in the standard way.
 */
void Board_init(Board* board){
	for (int x = 1; x <= Board_SIZE; x++){
		Board_setPiece(board, x, 2, Board_WHITE_PAWN);
		Board_setPiece(board, x, 7, Board_BLACK_PAWN);
	}
	Board_setPiece(board, 1, 1, Board_WHITE_ROOK);
	Board_setPiece(board, 2, 1, Board_WHITE_KNIGHT);
	Board_setPiece(board, 3, 1, Board_WHITE_BISHOP);
	Board_setPiece(board, 4, 1, Board_WHITE_KING);
	Board_setPiece(board, 5, 1, Board_WHITE_QUEEN);
	Board_setPiece(board, 6, 1, Board_WHITE_BISHOP);
	Board_setPiece(board, 7, 1, Board_WHITE_KNIGHT);
	Board_setPiece(board, 8, 1, Board_WHITE_ROOK);
	Board_setPiece(board, 1, 8, Board_BLACK_ROOK);
	Board_setPiece(board, 2, 8, Board_BLACK_KNIGHT);
	Board_setPiece(board, 3, 8, Board_BLACK_BISHOP);
	Board_setPiece(board, 4, 8, Board_BLACK_KING);
	Board_setPiece(board, 5, 8, Board_BLACK_QUEEN);
	Board_setPiece(board, 6, 8, Board_BLACK_BISHOP);
	Board_setPiece(board, 7, 8, Board_BLACK_KNIGHT);
	Board_setPiece(board, 8, 8, Board_BLACK_ROOK);
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 3; y <= 6; y++){
			Board_setPiece(board, x, y, Board_EMPTY);
		}
	}
	board->kingX[BLACK] = 4;
	board->kingX[WHITE] = 4;
	board->kingY[BLACK] = 8;
	board->kingY[WHITE] = 1;
}

/*
 * Clears the board from all pieces.
 */
void Board_clear(Board* board){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			board->matrix[x][y] = Board_EMPTY;
		}
	}	
}

/*
 * Populates a board according to another board.
 *
 * @params: (dest) - a pointer to the board to be populated, 
 *          (src)  - a pointer to the board according to whom (dest) will be populated
 */
void Board_copy(Board* dest, Board* src){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			dest->matrix[x][y] = src->matrix[x][y];
		}
	}
	dest->kingX[BLACK] = src->kingX[BLACK];
	dest->kingX[WHITE] = src->kingX[WHITE];
	dest->kingY[BLACK] = src->kingY[BLACK];
	dest->kingY[WHITE] = src->kingY[WHITE];
}

/*
 * Places a piece in a specified position on the board.
 *
 * @params: (x, y)  - the coordinates of the position to be populated
 *          (piece) - the piece to be placed on the board.
 */
void Board_setPiece(Board* board, int x, int y, char piece){
	board->matrix[x-1][y-1] = piece;
}

/*
 * Retrieves a piece from a specified position on the board.
 *
 * @params: (x, y)  - the coordinates of the position from which to retrieve the piece
 * @return: the piece in the specified position
 */
char Board_getPiece(Board* board, int x, int y){
	return board->matrix[x-1][y-1];
}

/*
 * Removes a piece from a specified position on the board.
 *
 * @params: (x, y) - the coordinates of the position from which the piece will be removed
 * @return: the removed piece
 */
char Board_removePiece(Board* board, int x, int y){
	char piece = Board_getPiece(board, x, y);
	board->matrix[x-1][y-1] = Board_EMPTY;
	return piece;
}

/*
 * Checks whether the input coordinates are within the range of the board's length and width.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 (ture) if the coordinates correspond to a valid position on the board, 0 (false) otherwise
 */
int Board_isInRange(int x, int y){
	if (x >= 1 && x <= Board_SIZE && y >= 1 && y <= Board_SIZE){
		return 1;
	}
	return 0;
}

/*
 * Checks whether the input coordinates correspond to an empty tile on the board.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 (true) if the coordinates correspond to an empty tile on the board, 0 (false) otherwise
 */
int Board_isEmpty(Board* board, int x, int y){
	return board->matrix[x-1][y-1] == Board_EMPTY;
}

/*
 * Determines the color of a piece in a given position.
 *
 * @params: (x, y) the coordinates of the given position
 * @return: -1 if the position is empty, the color of the piece otherwise
 */
static int Board_getColor(Board* board, int x, int y){
	if (Board_isEmpty(board, x, y)){
		return -1;
	}
	char piece = Board_getPiece(board, x, y);
	return piece == toupper(piece)? BLACK: WHITE;
}

void Board_updateKingPosition(Board* board, int x, int y){
	int piece = Board_getPiece(board, x, y);
	int player = Board_getColor(board, x, y);
	if (toupper(piece) == Board_BLACK_KING){
		board->kingX[player] = x;
		board->kingY[player] = y;
	}
}

/*
 * Updates a board according to a possible move.
 *
 * @params: (move) - the move to be carried out on the board 
 */
void Board_update(Board* board, struct PossibleMove* move){
	char piece = Board_getPiece(board, move->fromX, move->fromY);
	Board_removePiece(board, move->fromX, move->fromY);
	Board_setPiece(board, move->toX, move->toY, piece);
	Board_updateKingPosition(board, move->toX, move->toY);
	if (move->promotion != 0){
		Board_setPiece(board, move->toX, move->toY, move->promotion);
	}
}

/*
 * Creates a board representing the state of the board after a possible move has been carried out.
 *
 * @params: (possibleMove) - a pointer to the move to be carried out.
 * @return: NULL if any allocation errors occurred, the new board otherwise
 */
Board* Board_getPossibleBoard(Board* board, struct PossibleMove* possibleMove){
	Board* possibleBoard = Board_new();
	if (!possibleBoard){
		return NULL;
	}
	Board_copy(possibleBoard, board);
	Board_update(possibleBoard, possibleMove);
	return possibleBoard;
}

/*
 * Evaluates a single piece on the board according to the provided scoring function. 
 *
 * @params: (x,y) - the coordinates of the piece to be evaluated
 *			(color) - the color of the player the scoring function is adjusted for.
 */
int Board_evalPiece(Board* board, int x, int y, int player){
	char piece = Board_getPiece(board, x, y);
	int value = 0;
	switch (piece){
		case Board_BLACK_PAWN:   value = -1;   break;
		case Board_WHITE_PAWN:   value =  1;   break;
		case Board_BLACK_BISHOP: value = -3;   break;
		case Board_WHITE_BISHOP: value =  3;   break;
		case Board_BLACK_ROOK:   value = -5;   break;
		case Board_WHITE_ROOK:   value =  5;   break;
		case Board_BLACK_QUEEN:  value = -9;   break;
		case Board_WHITE_QUEEN:  value =  9;   break;
		case Board_BLACK_KNIGHT: value = -3;   break;
		case Board_WHITE_KNIGHT: value =  3;   break;
		case Board_BLACK_KING:	 value = -400; break;
		case Board_WHITE_KING:   value =  400; break;
	}
	if (player == BLACK){
		return -value;
	}
	return value;
}

/*
 * Evaluates the board according to the specified scoring function.
 *
 * @return: a numeric evaluation of the board
 */
int Board_getScore(Board* board, int player){
	int score = 0;
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			score += Board_evalPiece(board, x, y, player);
		}
	}
	return score;
}

static int canBeCapturedByAPawn(Board* board, int player){
	char enemyPawn = (player == BLACK)? Board_WHITE_PAWN: Board_BLACK_PAWN;
	int forward = (player == BLACK)? -1: 1;
	int x = board->kingX[player];
	int y = board->kingY[player];
	for (int i = -1; i <= 1; i+=2){
		if (Board_isInRange(x+i, y+forward)){
			if (Board_getPiece(board, x+i, y+forward) == enemyPawn){
				return 1;
			}
		}
	}
	return 0;
}

static int canBeCapturedByAKnight(Board* board, int player){
	char enemyKnight = (player == BLACK)? Board_WHITE_KNIGHT: Board_BLACK_KNIGHT;
	int x = board->kingX[player];
	int y = board->kingY[player];
	for (int sideward = -1; sideward <= 1; sideward+=2){
		for (int forward = -2; forward <= 2; forward+=4){
			if (Board_isInRange(x+sideward, y+forward)){
				if (Board_getPiece(board, x+sideward, y+forward) == enemyKnight){
					return 1;
				}
			}
		}
	}
	for (int sideward = -2; sideward <= 2; sideward+=4){
		for (int forward = -1; forward <= 1; forward+=2){
			if (Board_isInRange(x+sideward, y+forward)){
				if (Board_getPiece(board, x+sideward, y+forward) == enemyKnight){
					return 1;
				}
			}
		}
	}
	return 0;
}

static int canBeCapturedByABishopRookOrQueen(Board* board, int player){
	for (int sideward = -1; sideward <= 1; sideward++){
		for (int forward = -1; forward <= 1; forward++){
			for (int dist = 1; dist <= 7; dist++){
				int x = board->kingX[player]+dist*sideward;
				int y = board->kingY[player]+dist*forward;
				if (!Board_isInRange(x, y)){
					break;
				}
				if (Board_getColor(board, x, y) == player){
					break;
				}
				if (Board_isEmpty(board, x, y)){
					continue;
				}
				if (toupper(Board_getPiece(board, x, y)) == Board_BLACK_QUEEN){
					return 1;
				}
				if ((forward == 0 || sideward == 0) 
						&& toupper(Board_getPiece(board, x, y)) == Board_BLACK_ROOK){
					return 1;
				}
				if (toupper(Board_getPiece(board, x, y)) == Board_BLACK_BISHOP){
					return 1;
				}
			}
		}
	}
	return 0;
}

static int canBeCapturedByAKing(Board* board){
	return (abs(board->kingX[BLACK]-board->kingX[WHITE]) <= 1)
		&& (abs(board->kingY[BLACK]-board->kingY[WHITE]) <= 1);
}

int Board_isInCheck(Board* board, int player){
	return canBeCapturedByAPawn(board, player)
		|| canBeCapturedByAKnight(board, player)
		|| canBeCapturedByABishopRookOrQueen(board, player)
		|| canBeCapturedByAKing(board);
}	

/*
 * Checks if a given row is the furthest row for the given player.
 *
 * @params: (player) - the relevant player
			(y) - the relevant row number
 *
 * @return: 1 if (y) is the furthest row for (player), 0 otherwise 
 */
int Board_isFurthestRowForPlayer (int player, int y){
	if (((player == WHITE) && (y == 8)) || ((player == BLACK) && (y == 1))){
		return 1;
	}
	return 0;
}

static struct LinkedList* getPawnMoves(Board* board, int fromX, int fromY){
	int player = Board_getColor(board, fromX, fromY);
	char* promotionOptions = (player == WHITE)? 
			(char[4]){'q', 'b', 'n', 'r'}: (char[4]){'Q', 'B', 'N', 'R'};
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	
	if (!possibleMoves){
		return NULL;
	}
	
	int forward = (player == WHITE)? 1: -1;
	for (int sideward = -1; sideward <= 1; sideward++){
		int toX = fromX+sideward;
		int toY = fromY+forward;
		if (!Board_isInRange(toX, toY)){
			continue;
		}
		
		int canMoveForward = Board_isEmpty(board, toX, toY) && sideward == 0;
		int canCapture = Board_getColor(board, toX, toY) == !player && sideward != 0;
		if (canMoveForward || canCapture){
			if (Board_isFurthestRowForPlayer(player, toY)){	//generate all possible promotions	
				for (int i = 0; i <= 3; i++){
					if (PossibleMoveList_add(possibleMoves, fromX, fromY, toX, toY, promotionOptions[i], board) != 0){
						PossibleMoveList_free(possibleMoves);
						return NULL;
					}
				}
			}	
			else{
				if (PossibleMoveList_add(possibleMoves, fromX, fromY, toX, toY, 0, board) != 0){
					PossibleMoveList_free(possibleMoves);
					return NULL;
				}
			}
		}
	}
	return possibleMoves;
}

/*
 * Adds a single possible move from (fromX, fromY) to (fromX+sideward, fromY+forward) if 
 * this move is legal.
 *
 * @return: -1 if the given position is occupied or out of range, 
 *           0 otherwise
 */
int addMoveIfLegal(struct LinkedList* possibleMoves, Board* board, 
			int fromX, int fromY, int sideward, int forward){
	int player = Board_getColor(board, fromX, fromY);
	int toX = fromX+sideward;
	int toY = fromY+forward;
	if (!Board_isInRange(toX, toY)){
		return -1;
	}
	if (Board_getColor(board, toX, toY) == player){
		return -1;
	}
	struct PossibleMove* move = PossibleMove_new(fromX, fromY, toX, toY, 0, board); //Allocation error not handled
	if (!Board_isInCheck(move->board, player)){
		LinkedList_add(possibleMoves, move);
	}
	if (!Board_isEmpty(board, toX, toY)){
		return -1;
	}			
	return 0;
}

static struct LinkedList* getBishopMoves(Board* board, int fromX, int fromY){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	for (int sideward = -1; sideward <= 1; sideward += 2){
		for (int forward = -1; forward <= 1; forward += 2){
			for (int dist = 1; dist < Board_SIZE; dist++){
				int cantMoveFurther = addMoveIfLegal(possibleMoves, board, fromX, fromY, dist*sideward, dist*forward);
				if (cantMoveFurther){
					break;
				}
			}
		}
	}
	return possibleMoves;
}

static struct LinkedList* getRookMoves(Board* board, int fromX, int fromY){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	for (int sideward = -1; sideward <= 1; sideward += 2){
		for (int dist = 1; dist < Board_SIZE; dist++){
			int cantMoveFurther = addMoveIfLegal(possibleMoves, board, fromX, fromY, sideward*dist, 0);
			if (cantMoveFurther){
				break;
			}
		}
	}
	for (int forward = -1; forward <= 1; forward += 2){
		for (int dist = 1; dist < Board_SIZE; dist++){
			int cantMoveFurther = addMoveIfLegal(possibleMoves, board, fromX, fromY, 0, forward*dist);
			if (cantMoveFurther){
				break;
			}
		}
	}
	return possibleMoves;
}

static struct LinkedList* getQueenMoves(Board* board, int fromX, int fromY){
	struct LinkedList* possibleMoves1;
	struct LinkedList* possibleMoves2;
	possibleMoves1 = getBishopMoves(board, fromX, fromY);
	if (!possibleMoves1){
		return NULL;
	}
	possibleMoves2 = getRookMoves  (board, fromX, fromY);
	if (!possibleMoves2){
		PossibleMoveList_free(possibleMoves1);
		return NULL;
	}
	
	LinkedList_concatenate(possibleMoves1, possibleMoves2);
	return possibleMoves1;
}

static struct LinkedList* getKnightMoves(Board* board, int fromX, int fromY){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	for (int sideward = -1; sideward <= 1; sideward += 2){
		for (int forward = -2; forward <= 2; forward += 4){
			addMoveIfLegal(possibleMoves, board, fromX, fromY, sideward, forward);
		}
	}
	for (int sideward = -2; sideward <= 2; sideward += 4){
		for (int forward = -1; forward <= 1; forward += 2){
			addMoveIfLegal(possibleMoves, board, fromX, fromY, sideward, forward);
		}
	}
	return possibleMoves;
}

static struct LinkedList* getKingMoves(Board* board, int fromX, int fromY){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	for (int sideward = -1; sideward <= 1; sideward++){
		for (int forward = -1; forward <= 1; forward++){
			addMoveIfLegal(possibleMoves, board, fromX, fromY, sideward, forward);
		}
	}
	return possibleMoves;
}

struct LinkedList* Board_getPossibleMovesOfPiece(Board* board, int x, int y){
	char piece = Board_getPiece(board, x, y);
	switch (piece){
		case Board_BLACK_PAWN:
		case Board_WHITE_PAWN:   return getPawnMoves  (board, x, y);
		case Board_BLACK_BISHOP:
		case Board_WHITE_BISHOP: return getBishopMoves(board, x, y);
		case Board_BLACK_ROOK:
		case Board_WHITE_ROOK:   return getRookMoves  (board, x, y);
		case Board_BLACK_QUEEN:
		case Board_WHITE_QUEEN:  return getQueenMoves (board, x, y);
		case Board_BLACK_KNIGHT:
		case Board_WHITE_KNIGHT: return getKnightMoves(board, x, y);
		case Board_BLACK_KING:
		case Board_WHITE_KING:   return getKingMoves  (board, x, y);
	}
	return PossibleMoveList_new();
}


/*
 * Main function for getting all of the moves currently possible for a player. 
 *
 * @params: (player) - the player whose moves are to be put in the list
 * @return: a list of all moves currently possible for the player, or NULL if any allocation errors occurred 
 */
struct LinkedList* Board_getPossibleMoves(Board* board, int player){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			if (Board_getColor(board, x, y) != player){
				continue;
			}
			struct LinkedList* pieceMoves = Board_getPossibleMovesOfPiece(board, x, y);
			if (!pieceMoves){
				PossibleMoveList_free(possibleMoves);
				return NULL;
			}
			LinkedList_concatenate(possibleMoves, pieceMoves);	
		}
	}
	return possibleMoves;
}

/*
 * Auxiliary function for printing the lines as part of printing the playing board.
 */
static void printLine(){
	printf("  |");
	for (int x = 1; x < Board_SIZE*4; x++){
		printf("-");
	}
	printf("|\n");
}

/*
 * Prints an ASCII representation of the board.
 */
void Board_print(Board* board){
	printLine();
	for (int y = Board_SIZE-1; y >= 0 ; y--){
		printf((y < 9? " %d": "%d"), y+1);
		for (int x = 0; x < Board_SIZE; x++){
			printf("| %c ", board->matrix[x][y]);
		}
		printf("|\n");
		printLine();
	}
	printf("   ");
	for (int y = 0; y < Board_SIZE; y++){
		printf(" %c  ", (char)('a' + y));
	}
	printf("\n");
}

/*
 * Frees the structure.
 */
void Board_free(Board* board){
	free(board);
}