#ifndef CHESS_H
#define CHESS_H

#include "PossibleMove.h"
#include "Board.h"
#include "PossibleMoveList.h"
#include "PieceCounter.h"
#include "Iterator.h"
#include "LinkedList.h"
#include "GUI.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#define SETTINGS 0
#define GAME     1
#define BEST     0
#define CONSOLE 0
#define GUI 1
#define SINGLE_PLAYER_MODE 2
#define TWO_PLAYERS_MODE 1

#define str_equals(x, y) strcmp(x, y) == 0
#define toBlack(x) toupper(x)

Board board;
int displayMode;
int maxRecursionDepth;
int state;
int gameMode;
int player1;
int turn;
int first;
int counter[2][7];
int selectedX;
int selectedY;
LinkedList* movesOfSelectedPiece;
int gameEnded;
int isInCheck;
char modifyingPiece;
int settingInvalidPiece;
int kingIsMissing;

int pieceIsPawn(int x, int y);

#endif