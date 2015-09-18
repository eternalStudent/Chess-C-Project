#include <SDL.h>
#include <SDL_video.h>
#include "GUI.h"
#include "LinkedList.h"

Window window;

int GUI_init(){
	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);
	
	window = Window_new(768, 512);
	if(!window){
		return 1;
	}
	
	//create tree
}

int GUI_paint(){
	
}

Window* Window_new(int w, int h){
	Window window = (Window*)malloc(sizeof(Window));
	if(!window){
		return NULL;
	}
	window->surface = SDL_SetVideoMode(w, h, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (window->surface == NULL) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return NULL;
	}
	window->children = LinkedList_new(&Panel_free);
	if(!window->children){
		return NULL;
	}	
}

int Window_draw(void* data){
	Window * window = (Window*)data;
	Iterator iterator;
	Iterator_init(&iterator, window->children);
	while(Iterator_hasNext(&iterator)){
		Panel* next = (Panel*)Iterator_next(&iterator);
		int failure = Panel_draw(next);
		if(failure){
			return 1;
		}
	}
}

void Window_free(void* data){
	free (data);
}

Label* Label_new(SDL_Surface* image, SDL_Surface* parent, SDL_Rect rect, SDL_Rect sprite){
	Label* label = (Label*)malloc(sizeof(Label));
	if (!label){
		return NULL;
	}
	label->rect = rect;
	label->sprite = sprite;
	label->image = image;
	label->drawFunc = &Label_draw;
	label->parent = parent;
	label->backgroundColor = 0x7f7fff;
	return label;
}

int Label_draw(void* data){
	Label* label = (Label*)data;
	/* Set colorkey to BLUE*/
	if (SDL_SetColorKey(label->image, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 255)) != 0) {
		printf("ERROR: failed to set color key: %s\n", SDL_GetError());
		return 1;
	}
	if (SDL_BlitSurface(label->image, label->sprite, label->parent, label->rect) != 0) {
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		return 1;
	}

	return 0;
}

void Label_free(void* data){
	Label* label (Label*)data;
	SDL_FreeSurface(label->image);
	free(label);
}

Panel* Panel_new(SDL_Surface parent, SDL_Rect rect, int childrenType){//1 for Label, 2 for Button
	Panel panel = (Panel*)malloc(sizeof(Panel));
	if (!panel){
		return NULL;
	}
	panel->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, rect->w, rect>h, 32, 127, 127, 127, 255);
	if (!panel->surface){
		free(panel);
		return NULL;
	}
	panel->rect = rect;
	panel->parent = parent;	
	int isChildTypeLabel = (childrenType == 1); 
	panel->drawFunc = (isChildTypeLabel)? &Label_draw : &Button_draw;
	panel->children = (isChildTypeLabel)? LinkedList_new(&Label_free) : LinkedList_new(&Button_free);
	if (!panel->children){
		SDL_FreeSurface(panel->surface);
		free(panel);
		return NULL;
	}
	return panel;

}

int Panel_draw(void* data){
	Panel* panel = (Panel*)data;
	if (SDL_BlitSurface(panel->surface, 0, panel->parent, panel->rect) != 0) {
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		return 1;
	}
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		void* next = Iterator_next(&iterator);
		int failure = panel->drawFunc(next);
		if(failure){
			return 1;
		}
	}
	return 0;
}

void Panel_free(void* data){
	Panel* panel (Panel*)data;
	SDL_FreeSurface(panel->surface);
	SDL_FreeSurface(panel->parent);
	LinkedList_free(panel->children);
	free(panel);
}

Button* Button_new (SDL_Surface* parent, SDL_Rect rect, const char* normal, const char* hovered, const char* pressed){
	Button button = (Button*)malloc(sizeof(Button));
	if(!button){
		return NULL;
	}
	button->parent = parent;
	button->rect = rect;
	button->normal = SDL_LoadBMP(normal);
	if(!button->normal){
		free(button);
		return NULL;
	}
	button->hovered = SDL_LoadBMP(hovered);
	if(!button->normal){
		SDL_FreeSurface(button->normal);
		free(button);
		return NULL;
	}
	button->pressed = SDL_LoadBMP(pressed);
	if(!button->normal){
		SDL_FreeSurface(button->normal);
		SDL_FreeSurface(button->hovered);
		free(button);
		return NULL;
	}
	button->current = button->normal;
}

Button_setToNormal(Button* button){
	button->current = button->normal;
}

Button_setToHovered(Button* button){
	button->current = button->hovered;
}

Button_setToPressed(Button* button){
	button->current = button->pressed;
}

int Button_draw(void* data){
	Button* button = (Button*)data;
	if (SDL_BlitSurface(button->current, 0, button->parent, button->rect) != 0) {
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		return 1;
	}
	
}

