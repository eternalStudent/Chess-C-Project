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
int player1;
int maxRecursionDepth;
int state;
int gameMode;
struct LinkedList* humanPossibleMoves;
int turn;
int first;


/*
 * Checks whether an allocation has failed
 * 
 * @params: (ptr) - a pointer to the data that was allocated
 * @return: true (1) if the allocation has failed, false (0) otherwise 
 */
int allocationFailed(void* ptr){
	if (ptr == NULL){
		fprintf(stderr, "Error: standard function calloc has failed\n");
		return 1;
	}
	return 0;
}

/*
 * Initializes the global variables.
 */
void initialize(){
	board = Board_new();
	if (allocationFailed(board)){
		exit(0);
	}
	Board_init(board);
	player1 = WHITE;
	maxRecursionDepth = 1;
	state = SETTINGS;
	humanPossibleMoves = NULL;
	turn = player1;
	first = WHITE;
	gameMode = 1;
}

/*
 * Frees the allocated global variables.
 */
void freeGlobals(){
	Board_free(board);
	if (humanPossibleMoves != NULL){
		LinkedList_free(humanPossibleMoves);
	}
}

/*
 * Frees the allocated global variables and exit the program.
 */
void freeAndExit(){
	freeGlobals();
	exit(0);
}

int readTile(int* x, int* y){
	char str[64];
	char ch;
	scanf("%s", str);
	if (sscanf(str, "<%c,%d>", &ch, y) < 0){
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

int readPiece(){
	char colorString[6];
	char pieceString[7];
	int color;
	char piece;
	if (scanf("%5s %6s", colorString, pieceString) < 0){
		return -1;
	}
	color = stringToColor(colorString);
	if (color == -1){
		return -1;
	}
	piece = stringToPiece(pieceString, color);
	if (piece == 0){
		return -1;
	}
	return piece;
}

int removePiece(){
	int x, y;
	if (readTile(&x, &y) == -1){
		return -1;
	}
	if (!Board_isInRange(x, y)){
		return -2;
	}
	Board_removePiece(board, x, y);
	return 0;
}

int setPiece(){
	int x, y;
	if (readTile(&x, &y) == -1){
		return -1;
	}
	if (!Board_isInRange(x, y)){
		return -2;
	}
	char piece = readPiece();
	if (piece == -1){
		return -1;
	}
	Board_setPiece(board, x, y, piece);
	return 0;
}

int setGameMode(){
	int mode;
	if (scanf("%d", &mode) < 0){
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

int setDifficulty(){
	if (gameMode != 2){
		return -1;
	}
	
	char diff[6];
	scanf("%5s", diff);
	if (str_equals(diff, "best")){
		maxRecursionDepth = BEST;
		return 0;
	}
	if (!str_equals(diff, "depth")){
		return -1;
	}
	
	int depth;
	scanf("%d", &depth);
	if (depth < 1 || depth > 4){
		return -4;
	}
	maxRecursionDepth = depth;
	return 0;
}

int setUserColor(){
	if (gameMode != 2){
		return -1;
	}
	
	char colorString[6];
	scanf("%5s", colorString);
	int color = stringToColor(colorString);
	if (color == -1){
		return -1;
	}
	player1 = color;
	return 0;
}

int setFirstPlayer(){
	char colorString[6];
	scanf("%5s", colorString);
	int color = stringToColor(colorString);
	if (color == -1){
		return -1;
	}
	turn = color;
	return 0;
}

/* 
 * Performs a move on the board according to input from the user.
 * The move can consist of a single step, or several steps.
 *
 * @params: the input command string
 * @return: 00 if the move was carried out successfully
 *          01 if the command didn't match,  
 *          12 if the user input an illegal position on the board,
 *          14 if the initial tile doesn't contain one of the player's pieces, 
 *          15 if the move itself is illegal, 
 *          21 if any allocation errors occurred
 */ 
int movePiece(char* str){
	
}

/*
 * Updates the global variable (humanPossibleMoves).
 *
 * @return: 21 if any allocation errors occurred, 0 otherwise
 */
int updatePossibleMoves(){
	if (humanPossibleMoves){
		LinkedList_free(humanPossibleMoves);
		humanPossibleMoves = NULL;
	}
	
	humanPossibleMoves = Board_getPossibleMoves(board, player1);
	if (allocationFailed(humanPossibleMoves)){
		return 21;
	}
	return 0;
}

int printMovesOfPiece(){
	int x, y;
	if (readTile(&x, &y) == -1){
		return -1;
	}
	struct LinkedList* possibleMoves = Board_getPossibleMovesOfPiece(board, x, y);
	PossibleMoveList_print(possibleMoves);
	return 0;
}

/*
 * Executes a command given by the user
 *
 * @params: (command) - the command given by the user
 * @return: relevant exitcode
 */
int executeCommand(char* command){
	if (str_equals(command, "quit")){
		freeAndExit();
		return 0;
	}	
	if (state == SETTINGS){
		if (str_equals(command, "game_mode")){
			return setGameMode();
		}
		if (str_equals(command, "difficulty")){
			return setDifficulty();
		}
		if (str_equals(command, "user_color")){
			return setUserColor();
		}
		if (str_equals(command, "load")){
			//return loadGame();
		}
		if (str_equals(command, "clear")){
			Board_clear(board);
			return 0;
		}
		if (str_equals(command, "next_player")){
			return setFirstPlayer();
		}
		if (str_equals(command, "rm")){
			return removePiece();
		}
		if (str_equals(command, "set")){
			return setPiece();
		}
		if (str_equals(command, "print")){
			Board_print(board);
			return 0;
		}
		if (str_equals(command, "start")){
			turn = first;
			state = GAME;
			return 0;
		}
	}
	else{
		if (str_equals(command, "get_moves")){
			return printMovesOfPiece();
		}
	}
	scanf("\n"); //meant to go to the end of the line, not working though
	return -1;
}

/*
 * Prints relevant error message.
 *
 * @params: (error) - the exitcode of the error
 */
void printError(int error){
	switch (error){
		case  0: break;
		case -1: printf("Illegal command, please try again\n"); break;
		case -2: printf("Invalid position on the board\n"); break;
		case -3: printf("Wrong game mode\n"); break;
		case -4: printf("Wrong value for minimax depth. The value should be between 1 to 4\n"); break;
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
	printError(updatePossibleMoves());
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
		scanf("%63s", command);
		int error = executeCommand(command);
		printError(error);
	}
}

int main(){
	initialize();
	Board_print(board);
	printf("Enter game settings:\n");
	int gameOver = 0;
	while (!gameOver){
		humanTurn(turn);
		
		gameOver = (Board_getScore(board, turn) == -400);
	}
	printf("%s player wins!\n", (turn == BLACK)? "White" : "Black");
	freeGlobals();
	return 0;
}