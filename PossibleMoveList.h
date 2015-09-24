#ifndef POSSIBLEMOVELIST_H
#define POSSIBLEMOVELIST_H

#include "Board.h"
LinkedList* PossibleMoveList_new();

struct PossibleMove* PossibleMoveList_first(LinkedList* list);

int PossibleMoveList_add(LinkedList*, int, int, int, int, char, Board*);

void PossibleMoveList_print(LinkedList* list);

int PossibleMoveList_contains(LinkedList* list, PossibleMove* move);

void PossibleMoveList_free(LinkedList* list);
#endif