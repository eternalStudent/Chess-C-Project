#include "Iterator.c"
#include "PossibleMove.h"

struct LinkedList* PossibleMoveList_new();

struct PossibleMove* PossibleMoveList_first(struct LinkedList* list);

int PossibleMoveList_add(struct LinkedList*, int, int, int, int, char, char**);

void PossibleMoveList_print(struct LinkedList* list);

int PossibleMoveList_contains(struct LinkedList* list, struct PossibleMove* move);

void PossibleMoveList_free(struct LinkedList* list);