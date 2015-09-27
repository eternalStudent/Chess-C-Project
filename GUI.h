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
#define BACKGROUND_GREEN 0xabd383
#define NEW  0
#define LOAD 1
#define QUIT 2

typedef struct Window{
	SDL_Surface* surface;
	LinkedList* children;
	LinkedList* buttons;
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
	SDL_Rect rect;
	SDL_Rect normal;
	SDL_Rect hovered;
	SDL_Rect pressed;
	SDL_Rect current;
} Button;

typedef struct Radio{
	int state;
	int value;
	Label* label;
	struct RadioGroup* group;
} Radio;

typedef struct RadioGroup{
	LinkedList* radios;
	Radio* selected;
} RadioGroup;

int GUI_init();

int GUI_paint();

void Button_setToNormal(Button* button);

void Button_setToHovered(Button* button);

void Button_setToPressed(Button* button);

Button* getButtonByMousePosition(int x, int y);

void setAllButtonsToNormal();

int setScreenToGame();

#endif