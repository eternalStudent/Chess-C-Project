#include "Board.c"

/* 
 * Creates a new PossibleMove structure, consisting of the starting tile,
 * a list of tiles that are part of the move itself, and the state of the
 * board after this particular move has been carried out. 
 *
 * @params: start - a pointer to the starting tile
            end   - a pointer to the end tile 
 *          board - the board before the move
 * @return: NULL if any allocation errors occurred, the structure otherwise
 */
struct PossibleMove* PossibleMove_new(int fromX, int fromY, int toX, int toY, char** board){
	struct PossibleMove* move;
	move = (struct PossibleMove*)calloc(1, sizeof(struct PossibleMove));
	if (!move){
		return NULL;
	}
	move->fromX = fromX;
	move->fromY = fromY;
	move->toX = toX;
	move->toY = toY;
	move->promotion = 0;
	move->board = Board_getPossibleBoard(board, move);
	return move;
}

/*
 * Checks whether a PossibleMove struct represents the same move.
 *
 * @params: (other) - a pointer to the struct to be checked
 * @return: 1 (true) if both of the tiles represent the same move, 0 (false) otherwise
 */
int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other){
	return this->fromX == other->fromX && this->fromY == other->fromY
			&& this->toX == other->toX && this->toY == other->toY
			&& this->promotion == other->promotion;
}

/* 
 * Prints the structure in the format: "move <x,y> to <i,j>[<k,l>...]".
 */
void PossibleMove_print(struct PossibleMove* move){
	printf("move <%d,%c> to <%d,%c>", move->fromX, move->fromY+96, move->toX, move->fromY+96);
	switch(move->promotion){
		case 'b': printf("bishop\n"); break;
		case 'r': printf("rook\n"); break;
		case 'n': printf("knight\n"); break;
		case 'q': printf("queen\n"); break;
		default: printf("\n");
	}
}

/*
 * Deep-clones the move.
 *
 * @return: NULL if any allocation errors occurred, the cloned tile otherwise
 */
struct PossibleMove* PossibleMove_clone (struct PossibleMove* move){
	struct PossibleMove* clone;
	clone = (struct PossibleMove*)calloc(1, sizeof(struct PossibleMove));
	if (!clone){
		return NULL;
	}
	clone->fromX = move->fromX;
	clone->fromY = move->fromY;
	clone->toX = move->toX;
	clone->toY = move->toY;
	clone->promotion = move->promotion;
	
	char** boardClone = Board_new();
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
	struct PossibleMove* move = (struct PossibleMove*) data;
	Board_free(move->board);
	free(move);
}