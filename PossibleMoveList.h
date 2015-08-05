#ifndef POSSIBLEMOVELIST_H
#define POSSIBLEMOVELIST_H

#include "Board.h"
struct LinkedList* PossibleMoveList_new();

struct PossibleMove* PossibleMoveList_first(struct LinkedList* list);

int PossibleMoveList_add(struct LinkedList*, int, int, int, int, char, Board*);

void PossibleMoveList_print(struct LinkedList* list);

int PossibleMoveList_contains(struct LinkedList* list, struct PossibleMove* move);

void PossibleMoveList_free(struct LinkedList* list);
#endif