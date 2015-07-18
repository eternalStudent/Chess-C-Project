#include <stdio.h>
#include <ctype.h>
#include "Board.h"

/*
 * Creates a new board structure.
 *
 * @return:  NULL if an allocation error occurred, the new board structured as a two-dimensional char array otherwise
 */
char** Board_new(){
	char** board = calloc(Board_SIZE, sizeof(char*));
	if (!board){
		return NULL;
	}
	for(int i = 0; i < Board_SIZE; i++){
		board[i] = calloc(Board_SIZE, sizeof(char));
	}
	return board;
}

/*
 * Checks whether the input coordinates correspond to a black tile on the board.
 *
 * @params: (x, y) - the coordinates to be checked
 * @return: 1 (true) if the coordinates correspond to a black tile on the board, 0 (false) otherwise
 */
static int isOnBlack(int x, int y){
	return !((x+y)%2);
}

/*
 * Creates a new counter set structure, and initializes all counters according to the standard initial board configuration.
 *
 * @return:  NULL if an allocation error occurred, the new counter set structured as a set of ints otherwise
 */
struct counterSet* newCounterSet(){
	struct counterSet* res; 
	res = (struct counterSet*)malloc(sizeof(struct counterSet));
	if (!res){
		return NULL;
	}
	res->whitePawnCounter = 8;
	res->blackPawnCounter = 8;
	res->whiteBishopOnBlackTileCounter = 1;
	res->whiteBishopOnWhiteTileCounter = 1;
	res->blackBishopOnBlackTileCounter = 1;
	res->blackBishopOnWhiteTileCounter = 1;
	res->whiteRookCounter = 2;
	res->blackRookCounter = 2;
	res->whiteKnightCounter = 2;
	res->blackKnightCounter = 2;
	res->whiteQueenCounter = 1;
	res->blackQueenCounter = 1;
	res->whiteKingCounter = 1;
	res->blackKingCounter = 1;
	return res;
}

/*
 * Checks whether a piece can be legally added to the current board configuration.
 * @params: (pieceCounters) - the piece counters set
			(piece) - the type of piece to be updated
			(x, y) - the coordinates of the piece to be updated, used to ensure legal placement of bishops
 * @return:  1 if the can be legally added to the board, 0 otherwise
 */
int canPieceBeAdded(struct counterSet* pieceCounters, char piece, int x, int y){
	int exitcode = 0;
	int currCount;
	switch(piece){		
		case Board_BLACK_PAWN:   
			currCount = pieceCounters->blackPawnCounter;
			if (currCount <= 7){
				exitcode = 1;
			}		
			break;
		case Board_WHITE_PAWN: 
			currCount = pieceCounters->whitePawnCounter;
			if (currCount <= 7){
				exitcode = 1;
			}
			break;
		case Board_BLACK_BISHOP:
			currCount = (isOnBlack(x,y))? pieceCounters->blackBishopOnBlackTileCounter : pieceCounters->blackBishopOnWhiteTileCounter;
			if (currCount == 0){
				exitcode = 1;
			}			
			break;
		case Board_WHITE_BISHOP:
			currCount = (isOnBlack(x,y))? pieceCounters->whiteBishopOnBlackTileCounter : pieceCounters->whiteBishopOnWhiteTileCounter;
			if (currCount == 0){
				exitcode = 1;
			}		
			break;
		case Board_BLACK_ROOK:
			currCount = pieceCounters->blackRookCounter;
			if (currCount <= 1){
				exitcode = 1;
			}			
			break;
		case Board_WHITE_ROOK:
			currCount = pieceCounters->whiteRookCounter;
			if (currCount <= 1){
				exitcode = 1;
			}			
			break;
		case Board_BLACK_QUEEN:  
			currCount = pieceCounters->blackQueenCounter;
			if (currCount == 0){
				exitcode = 1;
			}			
			break;
		case Board_WHITE_QUEEN:
			currCount = pieceCounters->whiteQueenCounter;
			if (currCount == 0){
				exitcode = 1;
			}			
			break;
		case Board_BLACK_KNIGHT:
			currCount = pieceCounters->blackKnightCounter;
			if (currCount <= 1){
				exitcode = 1;
			}			
			break;
		case Board_WHITE_KNIGHT:
			currCount = pieceCounters->whiteKnightCounter;
			if (currCount <= 1){
				exitcode = 1;
			}			
			break;
		case Board_BLACK_KING:
			currCount = pieceCounters->blackKingCounter;
			if (currCount == 0){
				exitcode = 1;
			}			
			break;
		case Board_WHITE_KING:
			currCount = pieceCounters->whiteKingCounter;
			if (currCount == 0){
				exitcode = 1;
			}			
			break;
	}	
	return exitcode;	
}

/*
 * Updates the corresponding piece counter after a piece has been added/removed from the board.
 * @params: (pieceCounters) - the piece counters set
			(piece) - the type of piece to be updated
			(amountToAdd) - the amount of pieces of this type to be updated. Only used as 1 (when setting) or -1 (when removing)
			(x, y) - the coordinates of the piece to be updated, used to ensure legal placement of bishops
 * @return:  1 if it can be, 0 otherwise
 */
void updatePieceCounter(struct counterSet* pieceCounters, char piece, int amountToAdd, int x, int y){
	switch(piece){		
		case Board_BLACK_PAWN:   
			pieceCounters->blackPawnCounter += amountToAdd;
			break;
		case Board_WHITE_PAWN: 
			pieceCounters->whitePawnCounter += amountToAdd;
			break;
		case Board_BLACK_BISHOP:
			if(isOnBlack(x,y)){
				pieceCounters->blackBishopOnBlackTileCounter += amountToAdd;
			}
			else{
				pieceCounters->blackBishopOnWhiteTileCounter += amountToAdd;
			}		
			break;
		case Board_WHITE_BISHOP:
			if(isOnBlack(x,y)){
				pieceCounters->whiteBishopOnBlackTileCounter += amountToAdd;
			}
			else{
				pieceCounters->whiteBishopOnWhiteTileCounter += amountToAdd;
			}		
			break;
		case Board_BLACK_ROOK:
			pieceCounters->blackRookCounter += amountToAdd;
			break;
		case Board_WHITE_ROOK:
			pieceCounters->whiteRookCounter += amountToAdd;
			break;
		case Board_BLACK_QUEEN:  
			pieceCounters->blackQueenCounter += amountToAdd;
			break;
		case Board_WHITE_QUEEN:
			pieceCounters->whiteQueenCounter += amountToAdd;
			break;
		case Board_BLACK_KNIGHT:
			pieceCounters->blackKnightCounter += amountToAdd;
			break;
		case Board_WHITE_KNIGHT:
			pieceCounters->whiteKnightCounter += amountToAdd;
			break;
		case Board_BLACK_KING:
			pieceCounters->blackKingCounter += amountToAdd;
			break;
		case Board_WHITE_KING:
			pieceCounters->whiteKingCounter += amountToAdd;
			break;
	}	
}

/* Resets all piece counters to 0.
 * 
 * @params: (pieceCOunters) - the set of piece counters to be reset
 */
void resetCounters(struct counterSet* pieceCounters){
	pieceCounters->whitePawnCounter = 0;
	pieceCounters->blackPawnCounter = 0;
	pieceCounters->whiteBishopOnBlackTileCounter = 0;
	pieceCounters->whiteBishopOnWhiteTileCounter = 0;
	pieceCounters->blackBishopOnBlackTileCounter = 0;
	pieceCounters->blackBishopOnWhiteTileCounter = 0;
	pieceCounters->whiteRookCounter = 0;
	pieceCounters->blackRookCounter = 0;
	pieceCounters->whiteKnightCounter = 0;
	pieceCounters->blackKnightCounter = 0;
	pieceCounters->whiteQueenCounter = 0;
	pieceCounters->blackQueenCounter = 0;
	pieceCounters->whiteKingCounter = 0;
	pieceCounters->blackKingCounter = 0;
}

/*
 * Populates the board in the standard way.
 */
void Board_init(char** board){
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
}

/*
 * Clears the board from all pieces.
 */
void Board_clear(char** board){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			board[x][y] = Board_EMPTY;
		}
	}	
}

/*
 * Populates a board according to another board.
 *
 * @params: (dest) - a pointer to the board to be populated, 
 *          (src)  - a pointer to the board according to whom (dest) will be populated
 */
void Board_copy(char** dest, char** src){
	for (int x = 0; x < Board_SIZE; x++){
		for (int y = 0; y < Board_SIZE; y++){
			dest[x][y] = src[x][y];
		}
	}
}

/*
 * Places a piece in a specified position on the board.
 *
 * @params: (x, y)  - the coordinates of the position to be populated
 *          (piece) - the piece to be placed on the board.
 */
void Board_setPiece(char** board, int x, int y, char piece){
	board[x-1][y-1] = piece;
}

/*
 * Retrieves a piece from a specified position on the board.
 *
 * @params: (x, y)  - the coordinates of the position from which to retrieve the piece
 * @return: the piece in the specified position
 */
char Board_getPiece(char** board, int x, int y){
	return board[x-1][y-1];
}

/*
 * Removes a piece from a specified position on the board.
 *
 * @params: (x, y) - the coordinates of the position from which the piece will be removed
 * @return: the removed piece
 */
char Board_removePiece(char** board, int x, int y){
	char piece = Board_getPiece(board, x, y);
	board[x-1][y-1] = Board_EMPTY;
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
int Board_isEmpty(char** board, int x, int y){
	return board[x-1][y-1] == Board_EMPTY;
}

/*
 * Checks whether the input board is playable. Specifically, checks that both kings are on it.  
 *
 * @params: (board) - the board to be checked
			(pieceCounters) - the counters that keep track of how many pieces of each kind are on the board
 * @return: 1 (true) if the board is playable, 0 (false) otherwise
 */
int Board_isPlayable(char** board, struct counterSet* pieceCounters){
	int whiteKingOnBoard = pieceCounters->whiteKingCounter;
	int blackKingOnBoard = pieceCounters->blackKingCounter;
	if(!(whiteKingOnBoard && blackKingOnBoard)){
		return 0;
	}
	
	//TODO: handle initial configuration which results in an immediate tie or loss
	return 1;
}

/*
 * Moves a piece to a different tile in the board.
 *
 * @params: (oldX, oldY) - the coordinates of the piece to be moved
 *          (newX, newY) - the coordinates the piece will be moved to
 */
static int Board_move(char** board, int oldX, int oldY, int newX, int newY){
	char piece = Board_getPiece(board, oldX, oldY);
	Board_removePiece(board, oldX, oldY);
	Board_setPiece(board, newX, newY, piece);
	return 0;
}

/*
 * Updates a board according to a possible move.
 *
 * @params: (move) - the move to be carried out on the board 
 */
void Board_update(char** board, struct PossibleMove* move){
	Board_move(board, move->fromX, move->fromY, move->toX, move->toY);
}

/*
 * Creates a board representing the state of the board after a possible move has been carried out.
 *
 * @params: (possibleMove) - a pointer to the move to be carried out.
 * @return: NULL if any allocation errors occurred, the new board otherwise
 */
char** Board_getPossibleBoard(char** board, struct PossibleMove* possibleMove){
	char** possibleBoard = Board_new();
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
int Board_evalPiece(char** board, int x, int y, int player){
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
 * Determines the color of a piece in a given position.
 *
 * @params: (x, y) the coordinates of the given position
 * @return: -1 if the position is empty, the color of the piece otherwise
 */
static int Board_getColor(char** board, int x, int y){
	if (Board_isEmpty(board, x, y)){
		return -1;
	}
	char piece = Board_getPiece(board, x, y);
	return piece == toupper(piece)? BLACK: WHITE;
}

/*
 * Evaluates the board according to the specified scoring function.
 *
 * @return: a numeric evaluation of the board
 */
int Board_getScore(char** board, int player){
	int score = 0;
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			score += Board_evalPiece(board, x, y, player);
		}
	}
	return score;
}

static struct LinkedList* getPawnMoves(char** board, int fromX, int fromY, int check[2]){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	int player = Board_getColor(board, fromX, fromY);
	if (check[player]){
		return possibleMoves;
	}
	int forward = player == WHITE? 1: -1;
	for (int sideward = -1; sideward <= 1; sideward++){
		int toX = fromX+sideward;
		int toY = fromY+forward;
		if (!Board_isInRange(toX, toY)){
			continue;
		}
		
		int canMoveForward = Board_isEmpty(board, toX, toY) && sideward == 0;
		int canCapture = Board_getColor(board, toX, toY) == !player && sideward != 0;
		if (canMoveForward || canCapture){
			if (PossibleMoveList_add(possibleMoves, fromX, fromY, toX, toY, board) != 0){
				PossibleMoveList_free(possibleMoves);
				return NULL;
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

int addMoveIfLegal(struct LinkedList* possibleMoves, char** board, 
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
	PossibleMoveList_add(possibleMoves, fromX, fromY, toX, toY, board); //Allocation error not handled
	if (Board_getColor(board, toX, toY) == !player){
		return -1;
	}			
	return 0;
}

static struct LinkedList* getBishopMoves(char** board, int fromX, int fromY, int check[2]){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	int player = Board_getColor(board, fromX, fromY);
	if (check[player]){
		return possibleMoves;
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

static struct LinkedList* getRookMoves(char** board, int fromX, int fromY, int check[2]){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	int player = Board_getColor(board, fromX, fromY);
	if (check[player]){
		return possibleMoves;
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

static struct LinkedList* getQueenMoves(char** board, int fromX, int fromY, int check[2]){
	struct LinkedList* possibleMoves1;
	struct LinkedList* possibleMoves2;
	possibleMoves1 = getBishopMoves(board, fromX, fromY, check);
	if (!possibleMoves1){
		return NULL;
	}
	possibleMoves2 = getRookMoves  (board, fromX, fromY, check);
	if (!possibleMoves2){
		PossibleMoveList_free(possibleMoves1);
		return NULL;
	}
	
	if ((LinkedList_length(possibleMoves1) == 0) && LinkedList_length(possibleMoves2) != 0){
		LinkedList_free(possibleMoves1);
		return possibleMoves2;
	}
	
	LinkedList_concatenate(possibleMoves1, possibleMoves2);
	return possibleMoves1;
}

static struct LinkedList* getKnightMoves(char** board, int fromX, int fromY, int check[2]){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	int player = Board_getColor(board, fromX, fromY);
	if (check[player]){
		return possibleMoves;
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

static int causesMate(int kingX, int kingY, struct LinkedList* opponentMoves){
	struct Iterator iterator;
	Iterator_init(&iterator, opponentMoves);
	while (Iterator_hasNext(&iterator)){
		struct PossibleMove* opponentMove = (struct PossibleMove*)Iterator_next(&iterator);
		if (opponentMove->toX == kingX && opponentMove->toY == kingY){
			return 1;
		}
	}
	return 0;
}

static struct LinkedList* getKingMoves(char** board, int fromX, int fromY, int check[2]){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	int player = Board_getColor(board, fromX, fromY);
	for (int sideward = -1; sideward <= 1; sideward++){
		for (int forward = -1; forward <= 1; forward++){
			int toX = fromX+sideward;
			int toY = fromY+forward;
			if (!Board_isInRange(toX, toY) || Board_getColor(board, toX, toY) == player){
				continue;
			}
			struct PossibleMove* possibleMove = PossibleMove_new(fromX, fromY, toX, toY, board);
			if (!possibleMove){
				PossibleMoveList_free(possibleMoves);
				return NULL;
			}	
			if (check[player]){
				struct LinkedList* opponentMoves = 
						Board_getPossibleMoves(possibleMove->board, !player, check);
				if (!opponentMoves){
					PossibleMoveList_free(possibleMoves);
					PossibleMove_free(possibleMove);
					return NULL;
				}
				if (causesMate(toX, toY, opponentMoves)){
					continue;
				}
			}
			LinkedList_add(possibleMoves, possibleMove);
		}
	}
	return possibleMoves;
}

struct LinkedList* Board_getPossibleMovesOfPiece(char** board, int x, int y, int check[2]){
	char piece = Board_getPiece(board, x, y);
	switch (piece){
		case Board_BLACK_PAWN:
		case Board_WHITE_PAWN:   return getPawnMoves  (board, x, y, check);
		case Board_BLACK_BISHOP:
		case Board_WHITE_BISHOP: return getBishopMoves(board, x, y, check);
		case Board_BLACK_ROOK:
		case Board_WHITE_ROOK:   return getRookMoves  (board, x, y, check);
		case Board_BLACK_QUEEN:
		case Board_WHITE_QUEEN:  return getQueenMoves (board, x, y, check);
		case Board_BLACK_KNIGHT:
		case Board_WHITE_KNIGHT: return getKnightMoves(board, x, y, check);
		case Board_BLACK_KING:
		case Board_WHITE_KING:   return getKingMoves  (board, x, y, check);
	}
	return PossibleMoveList_new();
}

/*
 * Main function for getting all of the moves currently possible for a player. 
 *
 * @params: (player) - the player whose moves are to be put in the list
 * @return: a list of all moves currently possible for the player, or NULL if any allocation errors occurred 
 */
struct LinkedList* Board_getPossibleMoves(char** board, int player, int check[2]){
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			if (Board_isEmpty(board, x, y) || Board_getColor(board, x, y) != player){
				continue;
			}
			struct LinkedList* pieceMoves = Board_getPossibleMovesOfPiece(board, x, y, check);
			if (!pieceMoves){
				PossibleMoveList_free(possibleMoves);
				return NULL;
			}
			if (LinkedList_length(pieceMoves) == 0){
				continue;
			}
			if (LinkedList_length(possibleMoves) == 0) {
				LinkedList_free(possibleMoves);
				possibleMoves = pieceMoves;
			}
			else{
				LinkedList_concatenate(possibleMoves, pieceMoves);
			}		
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
void Board_print(char** board){

	printLine();
	for (int y = Board_SIZE-1; y >= 0 ; y--){
		printf((y < 9? " %d": "%d"), y+1);
		for (int x = 0; x < Board_SIZE; x++){
			printf("| %c ", board[x][y]);
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
void Board_free(char** board){
	for(int i = 0; i < Board_SIZE; i++){
		free(board[i]);
	}
	free(board);
}