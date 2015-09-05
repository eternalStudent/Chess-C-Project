#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "Board.h"
#include "PossibleMove.h"
#include "LinkedList.h"
#include "PossibleMoveList.h"


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
	Board_setPiece(board, 4, 1, Board_WHITE_QUEEN);
	Board_setPiece(board, 5, 1, Board_WHITE_KING);
	Board_setPiece(board, 6, 1, Board_WHITE_BISHOP);
	Board_setPiece(board, 7, 1, Board_WHITE_KNIGHT);
	Board_setPiece(board, 8, 1, Board_WHITE_ROOK);
	Board_setPiece(board, 1, 8, Board_BLACK_ROOK);
	Board_setPiece(board, 2, 8, Board_BLACK_KNIGHT);
	Board_setPiece(board, 3, 8, Board_BLACK_BISHOP);
	Board_setPiece(board, 4, 8, Board_BLACK_QUEEN);
	Board_setPiece(board, 5, 8, Board_BLACK_KING);
	Board_setPiece(board, 6, 8, Board_BLACK_BISHOP);
	Board_setPiece(board, 7, 8, Board_BLACK_KNIGHT);
	Board_setPiece(board, 8, 8, Board_BLACK_ROOK);
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 3; y <= 6; y++){
			Board_setPiece(board, x, y, Board_EMPTY);
		}
	}
	board->kingX[BLACK] = 5;
	board->kingX[WHITE] = 5;
	board->kingY[BLACK] = 8;
	board->kingY[WHITE] = 1;
	memset(board->hasKingEverMoved, 0, sizeof(board->hasKingEverMoved));
	memset(board->hasRookEverMoved, 0, sizeof(board->hasRookEverMoved[0][0]) * 2 * 2);
}

/*
 * @return: 1 if the piece located at (x,y) 
 * 			on the main playing board is a rook, 0 otherwise 
 */
int pieceIsRook(Board* board, int x, int y){
	char piece = Board_getPiece(board, x, y);
	return (piece == 'r' || piece == 'R');
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
	for (int i = 0; i < 2; i++){
		dest->kingX[i] = src->kingX[i];
		dest->kingY[i] = src->kingY[i];
		dest->hasKingEverMoved[i] = src->hasKingEverMoved[i];
		dest->hasRookEverMoved[i][0] = src->hasRookEverMoved[i][0];
		dest->hasRookEverMoved[i][1] = src->hasRookEverMoved[i][1];
	}
	
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
int Board_getColor(Board* board, int x, int y){
	if (Board_isEmpty(board, x, y)){
		return -1;
	}
	char piece = Board_getPiece(board, x, y);
	return piece == toupper(piece)? BLACK: WHITE;
}

/*
 * Updates the player's king's position in the kingX and kingY arrays, 
 * who keep track of both kings and are part of the board structure. 
 *
 * @params: (board) - the board struct for which the arrays are to be updated
 *			(x, y) the coordinates of the given position
 */
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
 * important! move->board may not be up to date!
 *
 * @params: (move) - the move to be carried out on the board 
 */
void Board_update(Board* board, struct PossibleMove* move){
	char piece = Board_getPiece(board, move->fromX, move->fromY);
	int player = Board_getColor(board, move->fromX, move->fromY);	
	if(move->toX != 0){ // non-castling move
		if(toupper(piece) == Board_BLACK_KING){ //keeping track of king movements for castling
			board->hasKingEverMoved[player] = 1;
		}	
		if(toupper(piece) == Board_BLACK_ROOK){ //keeping track of rook movements for castling
			int locationInRookArray = (move->fromX == 1)? 0 : 1;
			board->hasRookEverMoved[player][locationInRookArray] = 1;
		}
		Board_removePiece(board, move->fromX, move->fromY);
		Board_setPiece(board, move->toX, move->toY, piece);
		Board_updateKingPosition(board, move->toX, move->toY);
		if (move->promotion != 0){
			Board_setPiece(board, move->toX, move->toY, move->promotion);
		}
	}
	else{
		board->hasKingEverMoved[player] = 1;
		char rook = (player == WHITE)? Board_WHITE_ROOK: Board_BLACK_ROOK;
		char king = (player == WHITE)? Board_WHITE_KING: Board_BLACK_KING;
		int y = board->kingY[player];
		
		Board_removePiece(board, move->fromX, move->fromY);
		Board_removePiece(board, board->kingX[player], board->kingY[player]);
		
		if (move->fromX == 1){
			Board_setPiece(board, 3, y, king);
			Board_setPiece(board, 4, y, rook);
			board->kingX[player] = 3;
		}
		else{
			Board_setPiece(board, 7, y, king);
			Board_setPiece(board, 6, y, rook);
			board->kingX[player] = 7;
		}
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
 * Auxiliary function for checking whether a given player has any possible moves on a given board configuration. 
 *
 * @params: (player) - the relevant player 
 *			(board) - the relevant board configuration
 * @return: 1 if (player) has any possible moves on the (board) configuration, 0 otherwise
 */
int Board_possibleMovesExist (Board* board, int player){
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			if (Board_getColor(board, x, y) != player){
				continue;
			}
			struct LinkedList* pieceMoves = Board_getPossibleMovesOfPiece(board, x, y);
			if (!pieceMoves){
				return -1; //not sure how to handle possible allocation error
			}
			if (LinkedList_length(pieceMoves) > 0){
				PossibleMoveList_free(pieceMoves);
				return 1;
			}
			PossibleMoveList_free(pieceMoves);
		}
	}
	return 0;
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
	int playerHasPossibleMoves = Board_possibleMovesExist(board, player);
	if (playerHasPossibleMoves == -1){ //allocation error occured in Board_possibleMovesExist
		return -10001;
	}
	int opponentHasPossibleMoves = Board_possibleMovesExist(board, !player);
	if (opponentHasPossibleMoves == -1){
		return -10001;
	}
	if (!playerHasPossibleMoves){
		//losing configuration
		if (Board_isInCheck(board, player)){
			return -10000;
		}
		//tie
		else{
			return 0;
		}
	}
	//winning configuration
	if (Board_isInCheck(board, !player) && !opponentHasPossibleMoves){ 
		return 10000;
	}
	//otherwise
	int score = 0;
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			score += Board_evalPiece(board, x, y, player);
		}
	}
	return score;
}

int Board_evalMovesByPiece(Board* board, int x, int y, int player){
	int color = Board_getColor(board, x, y);
	if (color != player){
		return 0;
	}
	char piece = Board_getPiece(board, x, y);
	int value = 0;
	switch (piece){
		case Board_BLACK_PAWN:
		case Board_WHITE_PAWN:   value =  12;   break;
		case Board_BLACK_BISHOP:
		case Board_WHITE_BISHOP: value =  13;   break;
		case Board_BLACK_ROOK:
		case Board_WHITE_ROOK:   value =  14;   break;
		case Board_BLACK_QUEEN:
		case Board_WHITE_QUEEN:  value =  28;   break;
		case Board_BLACK_KNIGHT:
		case Board_WHITE_KNIGHT: value =  8;   break;
		case Board_BLACK_KING:
		case Board_WHITE_KING:   value =  8; break;
	}
	return value;
}

int Board_getUpperBoundMoves(Board* board, int player){
	int bound = 0;
	for (int x = 1; x <= Board_SIZE; x++){
		for (int y = 1; y <= Board_SIZE; y++){
			bound += Board_evalMovesByPiece(board, x, y, player);
		}
	}
	return bound;
}



/*
 * Checks if the player's king can be captured by an enemy pawn.
 * @params: (board) - the board to be checked
 *		    (player) - the player whose king is checked for possible captures
 *
 * @return: 1 if the player's king can be captured by an enemy pawn, 0 otherwise
 */
static int canBeCapturedByAPawn(Board* board, int player){
	char enemyPawn = (player == BLACK)? Board_WHITE_PAWN: Board_BLACK_PAWN;
	int forward = (player == BLACK)? -1 : 1;
	int x = board->kingX[player];
	int y = board->kingY[player];
	for (int i = -1; i <= 1; i += 2){
		if (Board_isInRange(x+i, y+forward)){
			if (Board_getPiece(board, x+i, y+forward) == enemyPawn){
				return 1;
			}
		}
	}
	return 0;
}

/*
 * Checks if the player's king can be captured by an enemy knight.
 * @params: (board) - the board to be checked
 *		    (player) - the player whose king is checked for possible captures
 *
 * @return: 1 if the player's king can be captured by an enemy knight, 0 otherwise
 */
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

/*
 * Checks if the player's king can be captured by an enemy bishop, rook or queen.
 * @params: (board) - the board to be checked
 *		    (player) - the player whose king is checked for possible captures
 *
 * @return: 1 if the player's king can be captured by an enemy knight, 0 otherwise
 */
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
				if (toupper(Board_getPiece(board, x, y)) != Board_BLACK_QUEEN &&
					toupper(Board_getPiece(board, x, y)) != Board_BLACK_ROOK &&
					toupper(Board_getPiece(board, x, y)) != Board_BLACK_BISHOP){
					break;
				}
				if (toupper(Board_getPiece(board, x, y)) == Board_BLACK_QUEEN){
					return 1;
				}
				if ((forward == 0 || sideward == 0) 
						&& toupper(Board_getPiece(board, x, y)) == Board_BLACK_ROOK){
					return 1;
				}
				if ((sideward != 0 && forward != 0) && toupper(Board_getPiece(board, x, y)) == Board_BLACK_BISHOP){
					return 1;
				}
			}
		}
	}
	return 0;
}

/*
 * Checks if the player's king can be captured by the other player's king.
 * @params: (board) - the board to be checked
 *		    (player) - the player whose king is checked for possible captures
 *
 * @return: 1 if the player's king can be captured by the other player's king, 0 otherwise
 */
static int canBeCapturedByAKing(Board* board){
	return (abs(board->kingX[BLACK]-board->kingX[WHITE]) <= 1)
		&& (abs(board->kingY[BLACK]-board->kingY[WHITE]) <= 1);
}

/*
 * Checks if the board is now in "check" for a given player,
 * by checking whether their king is at risk of being captured.
 * @params: (board) - the board to be checked
 *		    (player) - the player whose king is checked for possible captures
 *
 * @return: 1 if the player's king can be captured by an enemy piece, 0 otherwise
 */
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
 *			(y) - the relevant row number
 *
 * @return: 1 if (y) is the furthest row for (player), 0 otherwise 
 */
int Board_isFurthestRowForPlayer (int player, int y){
	if (((player == WHITE) && (y == 8)) || ((player == BLACK) && (y == 1))){
		return 1;
	}
	return 0;
}

/*
 * Gets all possible moves for a given pawn piece on a given board.
 * 
 * @params: (board) - a pointer to the relevant board
 *          (fromX, fromY) - location of the relevant pawn on (board)
 *
 * @return: A list of all possible moves for the relevant pawn.
 */
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
		char enemyKing = (player == WHITE)? 'K' : 'k';
		if (!Board_isInRange(toX, toY)){
			continue;
		}
		// the enemy king is never actually captured
		if (Board_getPiece(board, toX, toY) == enemyKing){
			continue;
		}
		
		int canMoveForward = Board_isEmpty(board, toX, toY) && sideward == 0;
		int canCapture = (Board_getColor(board, toX, toY) == !player) && (sideward != 0);
		if (canMoveForward || canCapture){
			if (Board_isFurthestRowForPlayer(player, toY)){	
				for (int i = 0; i <= 3; i++){  //generate all possible promotions
					struct PossibleMove* newMove1 = PossibleMove_new(fromX, fromY, toX, toY, promotionOptions[i], board);
					if (!newMove1){
						PossibleMoveList_free(possibleMoves);
						return NULL;
					}
					if (!Board_isInCheck(newMove1->board, player)){
						LinkedList_add(possibleMoves, newMove1);
					}
					else{
						PossibleMove_free(newMove1);
					}
				}
			}	
			else{
				struct PossibleMove* newMove2 = PossibleMove_new(fromX, fromY, toX, toY, 0, board);
				if (!newMove2){
					PossibleMoveList_free(possibleMoves);
					return NULL;
				}
				if (!Board_isInCheck(newMove2->board, player)){
					LinkedList_add(possibleMoves, newMove2);
				}
				else{
					PossibleMove_free(newMove2);
				}
			}
		}
	}
	return possibleMoves;
}

/*
 * Adds a single possible move from (fromX, fromY) to (fromX+sideward, fromY+forward)
 * to (possibleMoves) if  this move is legal.
 *
 * @return: -1 if the given position is occupied or out of range, 
 *           0 otherwise
 */
int addMoveIfLegal(struct LinkedList* possibleMoves, Board* board, 
			int fromX, int fromY, int sideward, int forward){
	int player = Board_getColor(board, fromX, fromY);
	int toX = fromX + sideward;
	int toY = fromY + forward;
	char enemyKing = (player == WHITE)? 'K' : 'k';
	if (!Board_isInRange(toX, toY)){
		return -1;
	}
	if (Board_getColor(board, toX, toY) == player){
		return -1;
	}
	// the enemy king is never actually captured
	if (Board_getPiece(board, toX, toY) == enemyKing){
		return -1;
	}
	struct PossibleMove* move = PossibleMove_new(fromX, fromY, toX, toY, 0, board); //Allocation error not handled
	if (!Board_isInCheck(move->board, player)){
		LinkedList_add(possibleMoves, move);
	}
	else {
		PossibleMove_free(move);
	}
	
	if (!Board_isEmpty(board, toX, toY)){
		return -1;
	}			
	return 0;
}

/*
 * Gets all possible moves for a given bishop piece on a given board.
 *
 * @return: A list of all possible moves for a bishop located at (fromX, fromY) on (board)       
 */
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

/*
 * Checks if a clear and safe horizontal path exists on (board) between (fromX, y) and (toX, y).
 * That is, a path that is not occupied by any piece, and if the king would move from (fromX, y) to (toX, y),
 * at no point will it be under attack.
 * @params: (board) - the board to be checked
 *		    (fromX), (toX), (y) - the relevant coordinates on the board
 *
 * @return: 1 if such a path exists, 0 otherwise
 */
int Board_clearAndSafeHorizontalPathExistsForKing(Board* board, int fromX, int toX, int y){
	int exitcode = 1;
	int player = Board_getColor(board, fromX, y);
	int adjustment = (toX < fromX)? -1 : 1;
	
	for (int i = 1; i <= 2; i++){
		if(!Board_isEmpty(board, fromX+i*adjustment, y)){
			exitcode = 0;
			break;
		}
		struct PossibleMove* step = PossibleMove_new(fromX+(i-1)*adjustment, y, fromX+adjustment*i, y, 0, board);
		if(!step){
			// allocation error not handled
		}
		if(Board_isInCheck(step->board, player)){
			exitcode = 0;
			PossibleMove_free(step);
			break;
		}
		PossibleMove_free(step);
	}
	return exitcode;
}

int Board_clearHorizontalPathExists(Board* board, int fromX, int toX, int y){
	int adjustment = (toX < fromX)? -1 : 1;
	for (int x = fromX+adjustment; x != toX; x += adjustment){
		if (!Board_isEmpty(board, x, y)){
			return 0;
		}
	}
	return 1;
}

static struct LinkedList* getCastlingMoves(Board* board, int x, int y){
	int player = Board_getColor(board, x, y);
	int legalY = (player == WHITE)? 1 : 8;
	
	struct LinkedList* possibleMoves = PossibleMoveList_new();
	if (!possibleMoves){
		return NULL;
	}
	if ((!pieceIsRook(board, x, y))
		||(y != legalY) 
		|| (board->hasKingEverMoved[player])
		|| (Board_isInCheck(board, player))
		|| (board->hasRookEverMoved[player][0] && board->hasRookEverMoved[player][1])){	
		return possibleMoves; //empty list
	}
	int kingDestX = (x == 1)? 3 : 7;
	int positionInRookMovementArray = (x == 1)? 0 : 1;
	if (Board_clearHorizontalPathExists(board, 5, x, y) 
		&& Board_clearAndSafeHorizontalPathExistsForKing(board, 5, kingDestX, y) 
		&& board->hasRookEverMoved[player][positionInRookMovementArray] == 0){
		struct PossibleMove* newCastlingMove = PossibleMove_new(x, y, 0, 0, 0, board);
		if(!newCastlingMove){
			return NULL;
		}
		LinkedList_add(possibleMoves, newCastlingMove);
	}
	return possibleMoves;
}

/*
 * Gets all possible moves for a given rook piece on a given board.
 *
 * @return: A list of all possible moves for a rook located at (fromX, fromY) on (board)       
 */
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
	struct LinkedList* castlingMoves = getCastlingMoves(board, fromX, fromY);
	if (!castlingMoves){
		PossibleMoveList_free(possibleMoves);
		return NULL;
	}
	LinkedList_concatenate(possibleMoves, castlingMoves);
	
	return possibleMoves;
}

/*
 * Gets all possible moves for a given queen piece on a given board.
 *
 * @return: A list of all possible moves for a queen located at (fromX, fromY) on (board)       
 */
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

/*
 * Gets all possible moves for a given knight piece on a given board.
 *
 * @return: A list of all possible moves for a knight located at (fromX, fromY) on (board)       
 */
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

/*
 * Gets all possible moves for a given king piece on a given board.
 *
 * @return: A list of all possible moves for a king located at (fromX, fromY) on (board)       
 */
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
	
	struct LinkedList* castlingMoves1 = getCastlingMoves(board, 1, fromY);
	if (!castlingMoves1){
		PossibleMoveList_free(possibleMoves);
		return NULL;
	}
	LinkedList_concatenate(possibleMoves, castlingMoves1);
	
	struct LinkedList* castlingMoves2 = getCastlingMoves(board, 8, fromY);
	if (!castlingMoves2){
		PossibleMoveList_free(possibleMoves);
		return NULL;
	}
	LinkedList_concatenate(possibleMoves, castlingMoves2);
	
	return possibleMoves;
}

/*
 * General function for getting all possible moves for a given piece on a given board.
 *
 * @return: A list of all possible moves for a piece located at (fromX, fromY) on (board)       
 */
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
		case Board_WHITE_KING: 	 return getKingMoves  (board, x, y);
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