#include "PossibleMove.h"
#include "Board.h"
#include "PossibleMoveList.h"
#include "PieceCounter.h"
#include "Iterator.h"
#include "LinkedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define SETTINGS 0
#define GAME     1
#define BEST     0
#define SINGLE_PLAYER_MODE 2
#define TWO_PLAYERS_MODE 1

#define str_equals(x, y) strcmp(x, y) == 0
#define toBlack(x) toupper(x)

Board board;
int maxRecursionDepth;
int state;
int gameMode;
int player1;
int turn;
int first;
int counter[2][7];


/*
 * Initializes the global variables.
 */
void initialize(){
	Board_init(&board);
	maxRecursionDepth = 1;
	state = SETTINGS;
	player1 = WHITE;
	turn = player1;
	first = WHITE;
	gameMode = 1;
	PieceCounter_setToMax(counter);
}

/*
 * Handles allocation errors by printing the relevant message and exiting the program.
 */
void allocationFailed(){
	fprintf(stderr, "Error: standard function calloc has failed\n");
	exit(0);
}

/*
 * The minimax AI algorithm.
 */
int alphabeta(struct PossibleMove* possibleMove, int depth, int player, int alpha, int beta){
	if (depth == 1){
		return Board_getScore(possibleMove->board, player);
	}
	Board* board = possibleMove->board;
	struct LinkedList* possibleMoves = Board_getPossibleMoves(board, player);
	//terminal node
	if (LinkedList_length(possibleMoves) == 0){
		LinkedList_free(possibleMoves);
		return Board_getScore(possibleMove->board, player);
	}
	//single child node
	if (LinkedList_length(possibleMoves) == 1){
		struct PossibleMove* onlyMove = PossibleMoveList_first(possibleMoves);
		LinkedList_freeAllButOne(possibleMoves, onlyMove);
		return Board_getScore(onlyMove->board, player);
	}
	
	int extremum = (player == player1)? INT_MIN: INT_MAX;;
	struct Iterator iterator;
	Iterator_init(&iterator, possibleMoves);
	while (Iterator_hasNext(&iterator)) {
		struct PossibleMove* currentPossibleMove = (struct PossibleMove*)Iterator_next(&iterator);
		int score = alphabeta(currentPossibleMove, depth-1, player, alpha, beta);
		if (	(player != player1 && score <  extremum) || 
				(player == player1 && score >  extremum) || 
				(rand()%2          && score == extremum)
			){
			extremum = score;
		}
		if (player == player1){
			alpha = (score > alpha)? score: alpha;
			if (alpha >= beta){
				break;
			}	
		}
		else{
			beta = (score < beta)? score: beta;
			if (beta <= alpha){
				break;
			}
		}
	}
	LinkedList_free(possibleMoves);
	return extremum;
}

/*
 * Retrieves a position on the board (tile) from user input.
 */
int readTile(char* str, int* x, int* y){
	char ch;
	if (sscanf(str, "<%c,%1d>", &ch, y) != 2){
		return -1;
	}
	if (*y > 8 || *y < 1 || ch < 97 || ch > 104){
		return -1;
	}
	*x = ch-96;
	return 0;
}

/*
 * @return: 1 if the piece located at (x,y) 
 * 			on the main playing board is a pawn, 0 otherwise 
 */
int pieceIsPawn(int x, int y){
	char piece = Board_getPiece(&board, x, y);
	return toBlack(piece) == Board_BLACK_PAWN; 
}

/*
 * @return: 1 if the piece located at (x,y) 
 * 			on the main playing board is a king, 0 otherwise 
 */
int pieceIsKing(int x, int y){
	char piece = Board_getPiece(&board, x, y);
	return toBlack(piece) == Board_BLACK_KING; 
}

/*
 * Retrieves a color (black or white) from user input.
 */
int stringToColor(char* str){
	if (str_equals(str, "black")){
		return BLACK;
	}
	if (str_equals(str, "white")){
		return WHITE;
	}
	return -1;
}

/*
 * Retrieves a piece type from user input.
 * 
 * @params: (str) - pointer to a user input string
 *			(color) - the relevant player's color
 */
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

/*
 * Main function for handling the "rm" command for removing pieces from the board during the settings stage. 
 *
 * @return: -1 if the input tile was not formatted legally
 *			-2 if the input tile is not a possible position on the board
 *			 0 otherwise
 */
int removePiece(char* command){
	int x, y;
	char tile[6];
	if (sscanf(command, "rm %5s", tile) != 1){
		return -1;
	}
	if (readTile(tile, &x, &y) == -1){
		return -1;
	}
	if (!Board_isInRange(x, y)){
		return -2;
	}
	
	char piece = Board_removePiece(&board, x, y);
	PieceCounter_update(counter, piece, -1, x, y);
	return 0;
}

/*
 * Main function for handling the "set" command for setting pieces on the board during the settings stage. 
 *
 * @return: -1 if the input tile, color or piece type were not formatted legally
 *			-2 if the input tile is not a possible position on the board
 *			-8 if the specified piece cannot be added there are already
 *			   too many pieces of this type on the board
 *			 0 otherwise
 */
int setPiece(char* command){
	int x, y;
	char tile[6];
	char colorString[6];
	char pieceString[7];
	if (sscanf(command, "set %5s %5s %6s", tile, colorString, pieceString) != 3){
		return -1;
	}
	
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
	
	char removedPiece = Board_getPiece(&board, x, y);
	PieceCounter_update(counter, removedPiece, -1, x, y);
	PieceCounter_update(counter, piece, 1, x, y);
	
	Board_setPiece(&board, x, y, piece);
	Board_updateKingPosition(&board, x, y);
	return 0;
}

/*
 * Main function for handling the "game_mode" command for setting the game mode during the settings stage. 
 *
 * @return: -1 if the input was not formatted legally
 *			-3 if a value other than 1 or 2 was input as game mode
 *			 0 otherwise
 */
int setGameMode(char* command){
	int mode;
	if (sscanf(command, "game_mode %d", &mode) < 0){
		return -1;
	}
	
	switch(mode){
		case 1: 
		gameMode = TWO_PLAYERS_MODE;
		printf("Running game in 2 players mode\n");
		break;
		case 2:
		gameMode = SINGLE_PLAYER_MODE;
		printf("Running game in player vs. AI mode\n");
		break;
		default: return -3;
	}
	return 0;
}

/*
 * Main function for handling the "difficulty" command for setting the difficulty level during the settings stage. 
 *
 * @return: -1 if the input was not formatted legally or if the current game mode is not player vs. AI
 *			-4 if an illegal number was input as the specified minimax depth
 *			 0 otherwise
 */
int setDifficulty(char* command){
	if (gameMode != 2){
		return -1;
	}
	
	char diff[6];
	int depth = -1; //initializing to non-relevant value to avoid problems with illegal inputs
	if(sscanf(command, "difficulty %5s %d", diff, &depth) < 1){
		return -1;
	}
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

/*
 * Main function for handling the "user_color" command for setting the human player's color during the settings stage.
 * Using this command is legal only if playing in game mode 2 (player vs. AI). 
 *
 * @return: -1 if the input was not formatted legally or if the current game mode is not player vs. AI
 *			 0 otherwise
 */
int setUserColor(char* command){
	if (gameMode != 2){
		return -1;
	}
	
	char colorString[6];
	if (sscanf(command, "user_color %5s", colorString) != 1){
		return -1;
	}
	int color = stringToColor(colorString);
	if (color == -1){
		return -1;
	}
	player1 = color;
	return 0;
}

/*
 * Main function for handling the "next_player" command for setting who's turn is first during the settings stage.
 *
 * @return: -1 if the input was not formatted legally
 *			 0 otherwise
 */
int setFirstPlayer(char* command){
	char colorString[6];
	if(sscanf(command, "next_player %5s", colorString) != 1){
		return -1;
	}
	int color = stringToColor(colorString);
	if (color == -1){
		return -1;
	}
	first = color;
	return 0;
}

/*
 * Main function for handling the "get_moves" command, for printing all possible moves for a given piece on the board during the game stage.
 *
 * @return: -5 if the input tile does not is not occupied by one of the current player's pieces
 *			-1 if the input tile was not formatted legally
 *			 1 if an allocation failure occurred
 *			 0 otherwise
 */
int printMovesOfPiece(char* command){
	char tile[6];
	if (sscanf(command, "get_moves %5s", tile) != 1){
		return -1;
	}
	int x, y;
	if (readTile(tile, &x, &y) == -1){
		return -1;
	}
	
	if (Board_getColor(&board, x, y) != turn){
		return -5;
	}
	
	struct LinkedList* possibleMoves = Board_getPossibleMovesOfPiece(&board, x, y);
	if (!possibleMoves){
		return 1;
	}
	
	PossibleMoveList_print(possibleMoves);
	PossibleMoveList_free(possibleMoves);
	return 0;
}

struct PossibleMove* readMove(char* command, int* exitcode){
	char fromTile[6];
	char toTile[6];
	char promoteToAsString[10];
	char promoteTo = 0; //initializing promoteTo to avoid problems with promotion-less move commands
	strncpy(promoteToAsString, "undefined", 10);
	
	*exitcode = 0;
	if (sscanf(command, "move %5s to %5s %6s", fromTile, toTile, promoteToAsString) < 2){
		*exitcode = -1;
		return NULL;
	}
	int fromX, fromY, toX, toY;
	if (readTile(fromTile, &fromX, &fromY) == -1 
			|| readTile(toTile, &toX, &toY) == -1){
		*exitcode = -1;
		return NULL;
	}
	
	if (!str_equals(promoteToAsString, "undefined")){
		promoteTo = stringToPiece(promoteToAsString, turn);
		if (!promoteTo){
			*exitcode = -1; // the promotion was not input legally
			return NULL;
		}
		if (promoteTo && (!Board_isFurthestRowForPlayer(turn, toY) || !pieceIsPawn(fromX, fromY))){
			*exitcode = -6; // the promotion was input legally, but the move itself is illegal
			return NULL;
		}
	}
	
	if(pieceIsPawn(fromX, fromY) && Board_isFurthestRowForPlayer(turn, toY) && promoteTo == 0){
		promoteTo = (turn = WHITE)? 'q':'Q';        //default promotion
	}
	
	if (!Board_isInRange(fromX, fromY) 
			|| !Board_isInRange(toX, toY)){
		*exitcode = -2;
		return NULL;
	}
	if (Board_getColor(&board, fromX, fromY) != turn){
		*exitcode = -5;
		return NULL;
	}
	
	struct PossibleMove* move = PossibleMove_new(fromX, fromY, toX, toY, promoteTo, &board);
	if (!move){
		*exitcode = 1;
		return NULL;
	}
	return move;
}

/*
 * Main function for handling the "move" command, for executing a move during the game stage.
 *
 * @return: -6 if the input move is illegal at this point in the game
 *			-5 if the input tile does not is not occupied by one of the current player's pieces
 *  		-2 if illegal coordinates are given as the origin or the destination of the move
 *			-1 if the input was not formatted legally
 *			 1 if an allocation failure occurred
 *			 0 otherwise
 */
int movePiece(char* command){
	int exitcode;
	struct PossibleMove* move = readMove(command, &exitcode);
	
	if (exitcode != 0){
		return exitcode;
	}
	
	struct LinkedList* possibleMoves = Board_getPossibleMovesOfPiece(&board, move->fromX, move->fromY);
	if (!possibleMoves){
		PossibleMove_free(move);
		return 1;
	}
	
	if (!PossibleMoveList_contains(possibleMoves, move)){
		PossibleMove_free(move);
		PossibleMoveList_free(possibleMoves);
		return -6;
	}
	
	Board_copy(&board, move->board);
	Board_print(&board);
	PossibleMove_free(move);
	PossibleMoveList_free(possibleMoves);
	turn = !turn;
	return 0;
}

int castleRook(char* command){
	char rookTile[6];
	if (sscanf(command, "castle %5s", rookTile) != 1){
		return -1;
	}
	int rookX, rookY;
	if (readTile(rookTile, &rookX, &rookY) == -1){
		return -1;
	}
	if (!Board_isInRange(rookX, rookY)){
		return -2;
	}
	if (Board_getColor(&board, rookX, rookY) != turn){
		return -5;
	}
	if(!pieceIsRook(&board, rookX, rookY)){
		return -11;
	}
	struct PossibleMove* castlingMove = PossibleMove_new(rookX, rookY, 0, 0, 0, &board);
	if (!castlingMove){
		return 1;
	}
	
	struct LinkedList* possibleMoves = Board_getPossibleMovesOfPiece(&board, rookX, rookY);
	if (!possibleMoves){
		PossibleMove_free(castlingMove);
		return 1;
	}
	
	if (!PossibleMoveList_contains(possibleMoves, castlingMove)){
		PossibleMove_free(castlingMove);
		PossibleMoveList_free(possibleMoves);
		return -12;
	}
	
	Board_copy(&board, castlingMove->board);
	Board_print(&board);
	PossibleMove_free(castlingMove);
	PossibleMoveList_free(possibleMoves);
	turn = !turn;
	return 0;
}

int computeBestDepth(int currentPlayer){
	int depth = 0;
	int switcher = 1;
	int bound = 1;
	int upperBoundCurrentPlayer = Board_getUpperBoundMoves(&board, currentPlayer);
	int upperBoundOpponent = Board_getUpperBoundMoves(&board, !currentPlayer);
	while (bound <= 1000000){
		if (switcher){
			bound *= upperBoundCurrentPlayer;
		}
		else {
			bound *= upperBoundOpponent;
		}
		depth += 1;
		switcher = !switcher;
	}
	return depth;
}

int printBestMoves(char* command){
	int depth;
	if (command[15] == 'b'){
		depth = computeBestDepth(turn);
	}
	else{
		if (sscanf(command, "get_best_moves %d", &depth) != 1){
			return -1;
		}
	}
	struct LinkedList* allPossibleMoves = Board_getPossibleMoves(&board, turn);
	if (!allPossibleMoves){
		return 1;
	}
	int bestScore = (turn == player1)? INT_MIN: INT_MAX;
	struct Iterator iterator;
	Iterator_init(&iterator, allPossibleMoves);
	struct LinkedList* bestMoves = PossibleMoveList_new();
	if (!bestMoves){
		PossibleMoveList_free(allPossibleMoves);
		return 1;
	}
	while(Iterator_hasNext(&iterator)){
		struct PossibleMove* currentMove = (struct PossibleMove*)Iterator_next(&iterator);
		int score = alphabeta(currentMove, depth, turn, INT_MIN, INT_MAX);
		if (    (score < bestScore && turn != player1) ||
				(score > bestScore && turn == player1)) {
			LinkedList_removeAll(bestMoves);
			LinkedList_add(bestMoves, currentMove);
			bestScore = score;
		}
		else if (score == bestScore){
			LinkedList_add(bestMoves, currentMove);
		}
	}
	PossibleMoveList_print(bestMoves);
	LinkedList_removeAll(bestMoves);	//frees the nodes
	LinkedList_free(bestMoves);			//frees the struct
	LinkedList_free(allPossibleMoves);	//frees the moves
	return 0;
}

int printMoveValue(char* command){
	int exitcode;
	int depth;
	int bestOffset = 0;
	if (command[10] == 'b'){
		depth = computeBestDepth(turn);
		bestOffset = 3;
	}
	else{
		if (sscanf(command, "get_score %d", &depth) < 0){
			return -1;
		}
	}
	
	if (!strstr(command, "move") && !strstr(command, "castle")){
		return -1;
	}
	
	if (strstr(command, "move")){
		struct PossibleMove* move = readMove(command + 12 + bestOffset, &exitcode);
		if (exitcode != 0){ // illegal input or illegal move
			return exitcode;
		}
		else{
			int score = alphabeta(move, depth, turn, INT_MIN, INT_MAX);
			printf("%d\n", score);
			PossibleMove_free(move);			
		}
	}
	// castling move
	else{
		int rookX, rookY;
		exitcode = readTile(command + 19, &rookX, &rookY); 
		if (exitcode == 0){
			struct PossibleMove* castlingMove = PossibleMove_new(rookX, rookY, 0, 0, 0, &board);
			int score = alphabeta(castlingMove, depth, turn, INT_MIN, INT_MAX);
			printf("%d\n", score);
			PossibleMove_free(castlingMove);	
		}
	}
	return exitcode;
}

/*
 * Main function for handling the "load" command, for loading a saved game during the settings stage.
 *
 * @return: -9 if an error occured during the opening of the file
 *			 0 otherwise
 */
int loadGame (char* command){
	int updatedGameMode = 0;
	char path[1024];
	char buff[51];
	if (sscanf(command, "%4s %1023s", buff, path) != 2){
		return -1;
	}
	
	FILE* gameFile = fopen(path, "r");
	if (!gameFile){
		return -9;
	}
	
	// resetting movement arrays, assuming files loaded without info about them correspond 
	// to a game where all of the kings and rooks have never moved  
	memset((&board)->hasKingEverMoved, 0, sizeof((&board)->hasKingEverMoved));
	memset((&board)->hasRookEverMoved, 0, sizeof((&board)->hasRookEverMoved[0][0]) * 2 * 2);
	
	while(fgets(buff, 50, gameFile) != 0){
		if (strstr (buff, "<next_turn>")){
			if (strstr(buff,"White")){
				first = WHITE;
			}
			else{
				first = BLACK;
			}			
		}
		else if (strstr (buff, "<game_mode>")){
			if (strstr(buff, "1")){
				gameMode = 1;
			}
			else{
				gameMode = 2;
			}
			updatedGameMode = 1;
		}

		else if (updatedGameMode && (gameMode == 2)){		
			if (strstr (buff, "<difficulty>")){
				if ((int)buff[13] <= 52){
					char difficultyAsChar = buff[13];
					int difficultyAsInt = (int)difficultyAsChar - 48;
					maxRecursionDepth = difficultyAsInt;					
				}
				else if (buff[13] == '<'){
					maxRecursionDepth = 1; //default value
				}
				else {
					maxRecursionDepth = BEST;
				}
			}	
			else if (strstr(buff, "<user_color>")){
				if (strstr(buff, "Black")){
					player1 = BLACK;
				}
				else{
					player1 = WHITE;
				}
			}
		}
		else if (strstr(buff, "row")){
			int y = (int)buff[7] - 48;
			for (int x = 1; x <= 8; x++){
				char piece = buff[8+x];
				if (piece != '_'){
					Board_setPiece(&board, x, y, piece);
				}
				else{
					Board_setPiece(&board, x, y, Board_EMPTY);
				}
				Board_updateKingPosition(&board, x, y);
			}
		}
		else if (strstr(buff, "kingMovementArray")){
			for (int i = 21; i <= 22; i++){
				(&board)->hasKingEverMoved[i-21] = buff[i]-48;
			}
		}
		else if (strstr(buff, "rookMovementArray")){
			for (int i = 21; i <= 22; i++){
				(&board)->hasRookEverMoved[0][i-21] = buff[i]-48;
			}
			for (int i = 23; i <= 24; i++){
				(&board)->hasRookEverMoved[1][i-23] = buff[i]-48;
			}
		}
	}
	fclose(gameFile);
	Board_print(&board);
	printf("Enter game settings:\n");
	return 0;
}

/*
 * Main function for handling the "save" command, for saving a game during the game stage.
 *
 * @return: -9 if an error occured during the saving of the file or if the specified name does not end with ".xml"
 *			 0 otherwise
 */
int saveGame (char* command){
	char fileName[1024];
	char buff[4];
	if (sscanf(command, "%4s %1023s", buff, fileName) != 2){
		return -1;
	}
	if (!strstr(fileName, ".xml")){
		return -9;
	}
	
	FILE* gameFile = fopen(fileName, "w");
	if (!gameFile){
		return -9;
	}
	
	char* nextTurn = (turn == WHITE)? "White":"Black";
	char gameModeAsChar = (char)(gameMode + 48);
	char* userColor = "";
	char* difficultyString = "";
	if (gameMode == 2){
		userColor = (player1 == WHITE)? "White":"Black";
		char difficultyAsChar = (char)(maxRecursionDepth + 48);
		difficultyString = (maxRecursionDepth == BEST)? "best" : &difficultyAsChar;
	}
	
	fprintf(gameFile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<game>\n\t<next_turn>%s" 
				      "</next_turn>\n\t<game_mode>%c</game_mode>\n\t<difficulty>%s</difficulty>\n\t<user_color>"
					  "%s</user_color>\n\t<board>\n\t\t", nextTurn, gameModeAsChar, difficultyString, userColor);	
	for(int y = 8; y >= 1; y--){
		fprintf(gameFile, "<row_%d>", y);
		for (int x = 1; x <= 8; x++){
			char newPiece = Board_getPiece(&board, x, y);
			if (newPiece == Board_EMPTY){
				fprintf(gameFile, "_");
			}
			else{ 
				fprintf(gameFile, "%c", newPiece);
			}
		}
		if (y != 1){
			fprintf(gameFile, "</row_%d>\n\t\t", y);
		}
		else{
			fprintf(gameFile, "</row_%d>\n\t</board>\n\t<general>\n\t\t<kingMovementArray>", y);
		}
	}

	for (int i = 0; i <= 1; i++){
		fprintf(gameFile, "%d", (&board)->hasKingEverMoved[i]);
	}
	fprintf(gameFile, "</kingMovementArray>\n\t\t<rookMovementArray>");
	for (int i = 0; i <= 1; i++){
		for (int j = 0; j <= 1; j++){
			fprintf(gameFile, "%d", (&board)->hasRookEverMoved[i][j]);
		}
	}
	fprintf(gameFile, "</rookMovementArray>\n\t</general>\n</game>");
	
	if (ferror(gameFile)){
		fclose(gameFile);
		return -10;
	}
	
	fclose(gameFile);
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
		exit(0);
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
			return loadGame(command);
		}
		if (str_equals(str, "clear")){
			Board_clear(&board);
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
			Board_print(&board);
			return 0;
		}
		if (str_equals(str, "start")){
			if(PieceCounter_kingIsMissing(counter)){
				return -7;
			}	
			turn = first;
			state = GAME;
			return 2; //special value to break the humanTurn loop so the initial board will always be checked for immediate loss or tie conditions
		}
	}
	else{
		if (str_equals(str, "get_moves")){
			return printMovesOfPiece(command);
		}
		if (str_equals(str, "move")){
			return movePiece(command);
		}
		if (str_equals(str, "castle")){
			return castleRook(command);
		}
		if (str_equals(str, "get_best_moves")){
			return printBestMoves(command);
		}
		if (str_equals(str, "get_score")){
			return printMoveValue(command);
		}	
		if (str_equals(str,"save")){
			return saveGame(command);
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
		case  1: allocationFailed();
		case  0: break;
		case -1: printf("Illegal command, please try again\n"); break;
		case -2: printf("Invalid position on the board\n"); break;
		case -3: printf("Wrong game mode\n"); break;
		case -4: printf("Wrong value for minimax depth. The value should be between 1 to 4\n"); break;
		case -5: printf("The specified position does not contain your piece\n"); break;
		case -6: printf("Illegal move\n"); break;
		case -7: printf("Wrong board initialization\n"); break;
		case -8: printf("Setting this piece creates an invalid board\n"); break;
		case -9: printf("Wrong file name\n"); break;
		case -10:printf("Error: standard function fprintf has failed\n"); break;
		case -11:printf("Wrong position for a rook\n"); break;
		case -12:printf("Illegal castling move\n"); break;
	}
}

struct PossibleMove* minimax(){
	struct LinkedList* allPossibleMoves = Board_getPossibleMoves(&board, turn);
	struct Iterator iterator;
	Iterator_init(&iterator, allPossibleMoves);
	int bestScore = INT_MIN;
	struct PossibleMove* bestMove = NULL;
	int bestDepth = 0;
	if (maxRecursionDepth == BEST){
		bestDepth = computeBestDepth(turn);
	}
	while(Iterator_hasNext(&iterator)){
		struct PossibleMove* currentMove = (struct PossibleMove*)Iterator_next(&iterator);
		int score = (maxRecursionDepth == BEST)? alphabeta(currentMove, bestDepth, turn, INT_MIN, INT_MAX) : alphabeta(currentMove, maxRecursionDepth, turn, INT_MIN, INT_MAX);
		if (score > bestScore || (score == bestScore && rand()%2)){
			bestScore = score;
			if (bestMove){
				PossibleMove_free(bestMove);
			}
			bestMove = PossibleMove_clone(currentMove); // possible allocation error
		}
	}
	LinkedList_free(allPossibleMoves);
	return bestMove;
}

/*
 * The computer turn procedure.
 */
void computerTurn(){
	struct PossibleMove* bestMove = minimax();
	printf("Computer: ");
	PossibleMove_print(bestMove);
	printf("\n");
	Board_update(&board, bestMove);
	PossibleMove_free(bestMove);
	turn = !turn;
	Board_print(&board);
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
		if (error == 2){ //breaks the loop when the "start" command is entered, so that the initial board will always be checked for losing/tie conditions
			break;
		}
		printError(error);
	}
}

int isEndGame(){
	int endGame = 0;
	struct LinkedList* allPossibleMoves = Board_getPossibleMoves(&board, turn);
	if (!allPossibleMoves){
		allocationFailed();
	}
	if (Board_isInCheck(&board, turn)){
		printf("Check\n");
	}
	if (LinkedList_length(allPossibleMoves) == 0){
		endGame = 1;
	}
	PossibleMoveList_free(allPossibleMoves);
	return endGame;
}

void printEndGameResults(){
	if (Board_isInCheck(&board, turn)){
		printf("Mate! %s player wins the game\n", (turn == BLACK)? "White" : "Black");
	}
	else{
		printf("The game ends in a tie\n");
	}
}

int main(){
	initialize();
	Board_print(&board);
	printf("Enter game settings:\n");
	
	while (1){
		if (isEndGame()){
			break;
		}
		if (turn != player1 && gameMode == SINGLE_PLAYER_MODE){
			computerTurn();
		}
		else{
			humanTurn(turn);
		}	
	}
	
	printEndGameResults();
	return 0;
}