#include "PossibleMoveList.h"
#include "PossibleMove.h"
#include "Iterator.h"
#include "LinkedList.h"

/*
 * Creates new LinkedList instant of PossibleMoves.
 *
 * @return: NULL if any allocation errors occurred, the list otherwise
 */
struct LinkedList* PossibleMoveList_new(){
	return LinkedList_new(&PossibleMove_free);
}

/*
 * @return: the first element in the list
 */
struct PossibleMove* PossibleMoveList_first(struct LinkedList* list){
	return (struct PossibleMove*)LinkedList_first(list);
}

int PossibleMoveList_add(struct LinkedList* list, int fromX, int fromY, 
		int toX, int toY, char promoteTo, Board* board){
	struct PossibleMove* move = PossibleMove_new(fromX, fromY, toX, toY, promoteTo, board);
	if (!move){
		return -1;
	}
	return LinkedList_add(list, move);
}

/*
 * Prints the list.
 */
void PossibleMoveList_print(struct LinkedList* list){
	struct Iterator iterator;
	Iterator_init(&iterator, list);
	while(Iterator_hasNext(&iterator)){
		struct PossibleMove* move = (struct PossibleMove*)Iterator_next(&iterator);
		PossibleMove_print(move);
	}
}

/*
 * Check whether a certain PossibleMove instant is in the list.
 *
 * @params: (move) - a pointer to the element to be checked
 * @return: 0 (false) if no such element found, 1 (true) otherwise
 */
int PossibleMoveList_contains(struct LinkedList* list, struct PossibleMove* move){
	struct Iterator iterator;
	Iterator_init(&iterator, list);
	while(Iterator_hasNext(&iterator)){
		struct PossibleMove* current = Iterator_next(&iterator);
		if (PossibleMove_equals(current, move)){
			return 1;
		}
	}
	return 0;
}

/* 
 * Frees the list.
 */
void PossibleMoveList_free(struct LinkedList* list){
	LinkedList_free(list);
}