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

typedef struct Window{
	SDL_Surface* surface;
	LinkedList* children;
	LinkedList* buttons;
} Window;

typedef struct Panel{
	SDL_Rect rect;
	SDL_Surface* surface;
	int (*drawFunc)(void*);
	LinkedList* children;
	SDL_Surface* parent;
} Panel;

typedef struct Label{
	SDL_Rect rect;
	SDL_Rect sprite;
	SDL_Surface* image;
	int (*drawFunc)(void*);
	SDL_Surface* parent;
	Uint32 backgroundColor;
} Label;

typedef struct Button{
	int id;
	SDL_Surface* parent;
	SDL_Rect rect;
	SDL_Surface* normal;
	SDL_Surface* hovered;
	SDL_Surface* pressed;
	SDL_Surface* current;
} Button;

int GUI_init();

int GUI_paint();

int getButtonIdByMousePosition(int x, int y)

#endif