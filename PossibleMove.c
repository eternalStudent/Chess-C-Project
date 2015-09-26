#include <stdlib.h>
#include <stdio.h>
#include "PossibleMove.h"
#include "Board.h"

/* 
 * Creates a new PossibleMove structure, consisting of the starting tile,
 * a list of tiles that are part of the move itself, and the state of the
 * board after this particular move has been carried out. 
 *
 * @params: start - a pointer to the starting tile
 *          end   - a pointer to the end tile 
 *          board - the board before the move
 * @return: NULL if any allocation errors occurred, the structure otherwise
 */
PossibleMove* PossibleMove_new(int fromX, int fromY, int toX, int toY, char promoteTo, Board* board){
	PossibleMove* move;
	move = (PossibleMove*)calloc(1, sizeof(PossibleMove));
	if (!move){
		return NULL;
	}
	move->fromX = fromX;
	move->fromY = fromY;
	move->toX = toX;
	move->toY = toY;
	move->promotion = promoteTo;
	move->board = Board_getPossibleBoard(board, move);
	if (!move->board){
		return NULL;
	}
	return move;
}

/*
 * Checks whether two different PossibleMove structs represent the same move.
 *
 * @params: (other) - a pointer to the struct to be checked
 * @return: 1 (true) if both of the tiles represent the same move, 0 (false) otherwise
 */
int PossibleMove_equals(PossibleMove* this, PossibleMove* other){
	return this->fromX == other->fromX && this->fromY == other->fromY
			&& this->toX == other->toX && this->toY == other->toY;
}

/* 
 * Prints the structure in the format: "move <fromX,fromY> to <toX,toY> promotion".
 */
void PossibleMove_print(PossibleMove* move){
	if (move->toX != 0){
		printf("<%c,%d> to <%c,%d>", move->fromX+96, move->fromY, move->toX+96, move->toY);
		switch(move->promotion){
			case 'b':
			case 'B': printf(" bishop\n"); break;
			case 'r': 
			case 'R': printf(" rook\n"); break;
			case 'n': 
			case 'N': printf(" knight\n"); break;
			case 'q':
			case 'Q': printf(" queen\n"); break;
			default: printf("\n");
		}
	}
	else { //castling move
		printf("castle <%c,%d>\n", move->fromX+96, move->fromY);
	}
}

/*
 * Deep-clones the move.
 *
 * @return: NULL if any allocation errors occurred, the cloned tile otherwise
 */
PossibleMove* PossibleMove_clone (PossibleMove* move){
	PossibleMove* clone;
	clone = (PossibleMove*)calloc(1, sizeof(PossibleMove));
	if (!clone){
		return NULL;
	}
	clone->fromX = move->fromX;
	clone->fromY = move->fromY;
	clone->toX = move->toX;
	clone->toY = move->toY;
	clone->promotion = move->promotion;
	
	Board* boardClone = Board_new();
	if (!boardClone){
		free(clone);
		return NULL;
	}
	Board_copy(boardClone, move->board);
	clone->board = boardClone;
	return clone;
}

/* 
 * Frees the structure.
 */
void PossibleMove_free(void* data){
	PossibleMove* move = (PossibleMove*) data;
	Board_free(move->board);
	free(move);
}