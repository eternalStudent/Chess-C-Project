typedef struct Window{
	SDL_Surface* surface;
	struct LinkedList* children;
} Window;

typedef struct Panel{
	SDL_Rect rect;
	SDL_Surface* surface;
	int (*drawFunc)(void*);
	struct LinkedList* children;
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
	SDL_Surface* parent;
	SDL_Rect rect;
	SDL_Surface* normal;
	SDL_Surface* hovered;
	SDL_Surface* pressed;
	SDL_Surface* current;
} Button;

