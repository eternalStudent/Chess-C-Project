#ifndef GUI_H
#define GUI_H

#include "SDL.h"
#include "SDL_video.h"
#include "LinkedList.h"
#include "Board.h"
#include "Iterator.h"
#include "Chess.h"

#define TILE_SIZE 64
#define BLACK_TILE_COLOR 0xd18b47
#define WHITE_TILE_COLOR 0xffce9e
#define BACKGROUND_WHITE 0xfffbf0
#define NEW  0
#define LOAD 1
#define QUIT 2
#define WHITE_KING 3
#define BLACK_KING 4
#define WHITE_QUEEN 5
#define BLACK_QUEEN 6
#define WHITE_KNIGHT 7
#define BLACK_KNIGHT 8
#define WHITE_PAWN 9
#define BLACK_PAWN 10
#define WHITE_ROOK 11
#define BLACK_ROOK 12
#define WHITE_BISHOP 13
#define BLACK_BISHOP 14
#define REMOVE_PIECE 15
#define CLEAR 16

typedef struct Window{
	SDL_Surface* surface;
	LinkedList* children;
	LinkedList* buttons;
	LinkedList* radios;
} Window;

typedef struct Panel{
	SDL_Rect rect;
	SDL_Surface* surface;
	int (*drawFunc)(struct Panel*);
	LinkedList* children;
	SDL_Surface* parent;
} Panel;

typedef struct Label{
	SDL_Rect pos;
	SDL_Rect crop;
	SDL_Surface* image;
	SDL_Surface* parent;
} Label;

typedef struct Button{
	int id;
	SDL_Surface* parent;
	SDL_Surface* img;
	SDL_Rect absoluteRect;
	SDL_Rect relativeRect;
	SDL_Rect normal;
	SDL_Rect hovered;
	SDL_Rect pressed;
	SDL_Rect current;
} Button;

typedef struct Radio{
	int state;
	int value;
	Label* label;
	SDL_Rect pos;
	struct RadioGroup* group;
} Radio;

typedef struct RadioGroup{
	LinkedList* radios;
	Radio* selected;
	int* parameter;
} RadioGroup;

int GUI_init();

int GUI_paint();

void Button_setToNormal(Button* button);

void Button_setToHovered(Button* button);

void Button_setToPressed(Button* button);

Button* getButtonByMousePosition(int x, int y);

Radio* getRadioByMousePosition(int x, int y);

void Radio_select(Radio* radio, int state);

void setAllButtonsToNormal();

int setScreenToGame();

#endif