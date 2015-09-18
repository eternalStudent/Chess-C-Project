#include <stdio.h>
#include <SDL.h>
#include <SDL_video.h>

#define WIN_W 512
#define WIN_H 512
#define TILE_SIZE 64


char board[9][9];
int selectX = 1;
int selectY = 1;

void paint(){
	SDL_Surface* w  = SDL_SetVideoMode(WIN_W, WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};
	SDL_Surface* img = SDL_LoadBMP("pieces.bmp");
	
	/* Set colorkey to BLUE*/
	if (SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 255)) != 0) {
		printf("ERROR: failed to set color key: %s\n", SDL_GetError());
		SDL_FreeSurface(img);
	}
	/* Clear window to BLACK*/
	if (SDL_FillRect(w,0,0) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
	}

	/* paint pieces */
 
	for (int x = 1; x <= 8; x++){
		for (int y = 1; y <= 8; y++){
      SDL_Rect piecePosition = {(x-1)*TILE_SIZE, (8-y)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
      int isBlackSquare = ((x+y) % 2 == 0);
			int r = (isBlackSquare)? 209 : 255;
      int g = (isBlackSquare)? 139 : 206;
      int b = (isBlackSquare)? 71 : 158;
      SDL_FillRect(w, &piecePosition, SDL_MapRGB(w->format, r, g, b));
      char piece = board[x][y];
			if (piece == ' '){
				continue;
			}
			SDL_BlitSurface(img, &rect, w, &piecePosition);
		}
	}
	SDL_Rect piecePosition = {(selectX-1)*TILE_SIZE, (8-selectY)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
	SDL_Rect selectRect = {0, 192, TILE_SIZE, TILE_SIZE};
	SDL_BlitSurface(img, &selectRect, w, &piecePosition);
	
	/* We finished drawing*/
	if (SDL_Flip(w) != 0) {
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
	}
	
	SDL_FreeSurface(img);
}

int main(void) {
	SDL_Event e;
	
	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);

		
	for (int x = 1; x <= 8; x++){
		for (int y = 1; y <= 8; y++){
			if (y == 1 || y == 2 || y == 7 || y == 8){
				board[x][y] = 'd';
			}
			else{
				board[x][y] = ' ';
			}
		}
	}	
	

	int quit = 0;
	while(!quit){
		paint();
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
				case (SDL_QUIT):
					quit = 1;
					break;
				case (SDL_KEYUP):
					if (e.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					break;
				case (SDL_MOUSEBUTTONUP):
					selectX = (e.button.x/TILE_SIZE)+1;
					selectY = 8-(e.button.y/TILE_SIZE);
					break;
				default:
					break;
			}
		}
	}

	
	return 0;
}
