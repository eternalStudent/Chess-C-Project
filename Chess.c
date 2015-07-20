#include "PossibleMove.c"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SETTINGS 0
#define GAME     1
#define UNDEFINED 101
#define BEST     0
#define str_equals(x, y) strcmp(x, y) == 0
#define toBlack(x) toupper(x)

char** board;
int maxRecursionDepth;
int state;
int gameMode;
int player1;
int turn;
int first;
int counter[2][7];
int kingX[2];
int kingY[2];


/*
 * Initializes the global variables.
 */
void initialize(){
	board = Board_new();
	if (!board){
		fprintf(stderr, "Error: standard function calloc has failed\n");
		exit(0);
	}
	Board_init(board);
	maxRecursionDepth = 1;
	state = SETTINGS;
	player1 = WHITE;
	turn = player1;
	first = WHITE;
	gameMode = 1;
	kingX[BLACK] = 4;
	kingY[BLACK] = 8;
	kingX[WHITE] = 4;
	kingY[BLACK] = 1;
	PieceCounter_setToMax(counter);
}

/*
 * Frees the allocated global variables.
 */
void freeGlobals(){
	Board_free(board);
}

/*
 * Frees the allocated global variables and exit the program.
 */
void freeAndExit(){
	freeGlobals();
	exit(0);
}

int readTile(char* str, int* x, int* y){
	char ch;
	if (sscanf(str, "<%c,%1d>", &ch, y) < 0){
		return -1;
	}
	*x = ch-96;
	return 0;
}

int stringToColor(char* str){
	if (str_equals(str, "black")){
		return BLACK;
	}
	if (str_equals(str, "white")){
		return WHITE;
	}
	return -1;
}

char stringToPiece(char* str, int color){
	char piece = 0;
	if (str_equals(str, "pawn")){
		piece = Board_WHITE_PAWN;
	}
	if (str_equals(str, "bishop")){
		piece = Board_WHITE_BISHOP;
	}
	if (str_equals(str, "rook")){
		piece = Board_WHITE_ROOK;
	}
	if (str_equals(str, "knight")){
		piece = Board_WHITE_KNIGHT;
	}
	if (str_equals(str, "queen")){
		piece = Board_WHITE_QUEEN;
	}
	if (str_equals(str, "king")){
		piece = Board_WHITE_KING;
	}
	if (color == BLACK){
		piece = toBlack(piece);
	}
	return piece;
}

int removePiece(char* command){
	int x, y;
	char tile[6];
	sscanf(command, "rm %5s", tile);
	if (readTile(tile, &x, &y) == -1){
		return -1;
	}
	if (!Board_isInRange(x, y)){
		return -2;
	}
	
	char piece = Board_getPiece(board, x, y);
	PieceCounter_update(counter, piece, -1, x, y);
	
	Board_removePiece(board, x, y);
	return 0;
}

int setPiece(char* command){
	int x, y;
	char tile[6];
	char colorString[6];
	char pieceString[7];
	sscanf(command, "set %5s %5s %6s", tile, colorString, pieceString);
	
	if (readTile(tile, &x, &y) == -1){
		return -1;
	}
	if (!Board_isInRange(x, y)){
		return -2;
	}
	int color = stringToColor(colorString);
	char piece = stringToPiece(pieceString, color);
	if (piece == 0 || color == -1){
		return -1;
	}
	if (PieceCounter_isAtMax(counter, piece, x, y)){
		return -8;
	}
	
	char removedPiece = Board_getPiece(board, x, y);
	PieceCounter_update(counter, removedPiece, -1, x, y);
	PieceCounter_update(counter, piece, 1, x, y);
	
	Board_setPiece(board, x, y, piece);
	if (toBlack(piece) == Board_BLACK_KING){
		kingX[color] = x;
		kingY[color] = y;
	}
	return 0;
}

int setGameMode(char* command){
	int mode;
	if (sscanf(command, "game_mode %d", &mode) < 0){
		return -1;
	}
	
	switch(mode){
		case 1: 
		gameMode = 1;
		printf("Running game in 2 players mode\n");
		break;
		case 2:
		gameMode = 2;
		printf("Running game in player vs. AI mode\n");
		break;
		default: return -3;
	}
	return 0;
}

int setDifficulty(char* command){
	if (gameMode != 2){
		return -1;
	}
	
	char diff[6];
	int depth;
	sscanf(command, "difficulty %5s %d", diff, &depth);
	if (str_equals(diff, "best")){
		maxRecursionDepth = BEST;
		return 0;
	}
	if (!str_equals(diff, "depth")){
		return -1;
	}
	
	if (depth < 1 || depth > 4){
		return -4;
	}
	maxRecursionDepth = depth;
	return 0;
}

int setUserColor(char* command){
	if (gameMode != 2){
		return -1;
	}
	
	char colorString[6];
	sscanf(command, "user_color %5s", colorString);
	int color = stringToColor(colorString);
	if (color == -1){
		return -1;
	}
	player1 = color;
	return 0;
}

int setFirstPlayer(char* command){
	char colorString[6];
	sscanf(command, "next_player %5s", colorString);
	int color = stringToColor(colorString);
	if (color == -1){
		return -1;
	}
	first = color;
	return 0;
}

int printMovesOfPiece(char* command){
	char tile[6];
	sscanf(command, "get_moves %5s", tile);
	int x, y;
	if (readTile(tile, &x, &y) == -1){
		return -1;
	}
	
	struct LinkedList* possibleMoves = Board_getPossibleMovesOfPiece(board, x, y);
	if (!possibleMoves){
		return 1;
	}
	
	PossibleMoveList_print(possibleMoves);
	PossibleMoveList_free(possibleMoves);
	return 0;
}

void updateKingPosition(int player, int x, int y){
	int piece = Board_getPiece(board, x, y);
	if (toBlack(piece) == Board_BLACK_KING){
		kingX[player] = x;
		kingY[player] = y;
	}
}

int movePiece(char* command){
	char fromTile[6];
	char toTile[6];
	//char promotion[7];
	sscanf(command, "move %5s to %5s", fromTile, toTile);
	int fromX, fromY, toX, toY;
	
	if (readTile(fromTile, &fromX, &fromY) == -1 
			|| readTile(toTile, &toX, &toY) == -1){
		return -1;
	}
	if (!Board_isInRange(fromX, fromY) 
			|| !Board_isInRange(toX, toY)){
		return -2;
	}
	if (Board_getColor(board, fromX, fromY) != turn){
		return -5;
	}
	
	struct PossibleMove* move = PossibleMove_new(fromX, fromY, toX, toY, board);
	if (!move){
		return 1;
	}
	
	struct LinkedList* possibleMoves = Board_getPossibleMovesOfPiece(board, fromX, fromY);
	if (!possibleMoves){
		PossibleMove_free(move);
		return 1;
	}
	
	if (!PossibleMoveList_contains(possibleMoves, move)){
		return -6;
	}
	
	Board_update(board, move);
	Board_print(board);
	PossibleMove_free(move);
	PossibleMoveList_free(possibleMoves);
	updateKingPosition(turn, toX, toY);
	turn = !turn;
	return 0;
}

/*
 * Executes a command given by the user
 *
 * @params: (command) - the command given by the user
 * @return: relevant exitcode
 */
int executeCommand(char* command){
	char str[64];
	sscanf(command, "%s", str);
	if (str_equals(str, "quit")){
		freeAndExit();
		return 0;
	}	
	if (state == SETTINGS){
		if (str_equals(str, "game_mode")){
			return setGameMode(command);
		}
		if (str_equals(str, "difficulty")){
			return setDifficulty(command);
		}
		if (str_equals(str, "user_color")){
			return setUserColor(command);
		}
		if (str_equals(str, "load")){
			//return loadGame(command);
		}
		if (str_equals(str, "clear")){
			Board_clear(board);
			PieceCounter_reset(counter);
			return 0;
		}
		if (str_equals(str, "next_player")){
			return setFirstPlayer(command);
		}
		if (str_equals(str, "rm")){
			return removePiece(command);
		}
		if (str_equals(str, "set")){
			return setPiece(command);
		}
		if (str_equals(str, "print")){
			Board_print(board);
			return 0;
		}
		if (str_equals(str, "start")){
			if(!Board_isPlayable(board, counter)){
				return -7;
			}	
			turn = first;
			state = GAME;
			return 0;
		}
	}
	else{
		if (str_equals(str, "get_moves")){
			return printMovesOfPiece(command);
		}
		if (str_equals(str, "move")){
			return movePiece(command);
		}
	}
	return -1;
}

/*
 * Prints relevant error message.
 *
 * @params: (error) - the exitcode of the error
 */
void printError(int error){
	switch (error){
		case  1: fprintf(stderr, "Error: standard function calloc has failed\n"); freeAndExit();
		case  0: break;
		case -1: printf("Illegal command, please try again\n"); break;
		case -2: printf("Invalid position on the board\n"); break;
		case -3: printf("Wrong game mode\n"); break;
		case -4: printf("Wrong value for minimax depth. The value should be between 1 to 4\n"); break;
		case -5: printf("The specified position does not contain your piece\n"); break;
		case -6: printf("Illegal move\n"); break;
		case -7: printf("Wrong board initialization\n"); break;
		case -8: printf("Setting this piece creates an invalid board\n"); break;
	}
}

/*
 * The minimax AI algorithm.
 */
struct PossibleMove* minimax(struct PossibleMove* possibleMove, int depth, int player){
	if (depth == 0){
		return possibleMove;
	}
	char** board = possibleMove->board;
	struct LinkedList* possibleMoves = Board_getPossibleMoves(board, player);
	if (LinkedList_length(possibleMoves) == 0){
		LinkedList_free(possibleMoves);
		return possibleMove;
	}
	if (LinkedList_length(possibleMoves) == 1){
		struct PossibleMove* onlyMove = PossibleMoveList_first(possibleMoves);
		LinkedList_freeAllButOne(possibleMoves, onlyMove);
		return onlyMove;
	}
	
	struct PossibleMove* bestPossibleMove;
	int extremum = UNDEFINED;
	struct Iterator iterator;
	Iterator_init(&iterator, possibleMoves);
	while (Iterator_hasNext(&iterator)) {
		struct PossibleMove* currentPossibleMove = (struct PossibleMove*)Iterator_next(&iterator);
		struct PossibleMove* temp = minimax(currentPossibleMove, depth-1, player);
		int score = Board_getScore(temp->board, player);
		if (currentPossibleMove != temp){
			PossibleMove_free(temp);
		}		
		if (extremum == UNDEFINED || 
				(player != player1 && score >  extremum) || 
				(player == player1 && score <  extremum) || 
				(rand()%2       && score == extremum)
			){
			extremum = score;
			bestPossibleMove = currentPossibleMove;
		}
	}
	LinkedList_freeAllButOne(possibleMoves, bestPossibleMove);
	return bestPossibleMove;
}

/*
 * The computer turn procedure.
 */
void computerTurn(){
	struct PossibleMove possibleMove;
	possibleMove.board = board;
	struct PossibleMove* bestMove = minimax(&possibleMove, maxRecursionDepth, !player1);
	printf("Computer: ");
	PossibleMove_print(bestMove);
	printf("\n");
	Board_update(board, bestMove);
	PossibleMove_free(bestMove);
	turn = !turn;
	Board_print(board);
}

/*
 * The human turn procedure
 */
void humanTurn(int player){
	while (turn == player){
		if (state == GAME){
			printf(player == BLACK? "Black": "White");
			printf(" player - enter your move:\n");
		}
		char command[64];
		fgets(command, 63, stdin);
		int error = executeCommand(command);
		printError(error);
	}
}

int main(){
	initialize();
	Board_print(board);
	printf("Enter game settings:\n");
	while (1){
		humanTurn(turn);
	}
	printf("Mate! %s player wins the game\n", (turn == BLACK)? "White" : "Black");
	freeGlobals();
	return 0;
}