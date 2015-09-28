#include "GUI.h"

Window* window;

//general functions

static int setBlueToTransparent(SDL_Surface* img){
	if (SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 127, 127, 255))) {
		printf("ERROR: failed to set color key: %s\n", SDL_GetError());
		SDL_FreeSurface(img);
		return 1;
	}
	return 0;
}

static SDL_Surface* loadImage(const char* path){
	SDL_Surface* img = SDL_LoadBMP(path);
	if (!img){
		printf("ERROR: failed to load BMP file: %s\n", SDL_GetError());
		return NULL;
	}
	if (setBlueToTransparent(img)){
		return NULL;
	}
	return img;
}

static int drawImage(SDL_Surface* img, SDL_Surface* surface, int x, int y){
	SDL_Rect pos = {x, y, img->w, img->h};
	if(SDL_BlitSurface(img, 0, surface, &pos)){
		printf("1ERROR: failed to blit image: %s\n", SDL_GetError());
		return 1;
	}	
	return 0;
}

static int drawImageByPath(const char* path, SDL_Surface* surface, int x, int y){
	SDL_Surface* img = loadImage(path);
	if (!img){
		SDL_FreeSurface(img);
		return 1;
	}	
	if(drawImage(img, surface, x, y)){
		SDL_FreeSurface(img);
		return 1;
	}
	SDL_FreeSurface(img);
	return 0;
}

static int drawSubImage(SDL_Surface* img, SDL_Rect crop, SDL_Surface* surface, SDL_Rect pos){
	if (SDL_BlitSurface(img, &crop, surface, &pos)) {
		printf("2ERROR: failed to blit image: %s\n", SDL_GetError());
		return 1;
	}
	return 0;
}

static int fillBackground(Panel* panel){
	if (SDL_FillRect(panel->surface, 0, BACKGROUND_WHITE) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return 1;
	}
	return 0;
}

static int flipBuffer(SDL_Surface* surface){
	if (SDL_Flip(surface)) {
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		return 1;
	}
	return 0;
}

static SDL_Rect Rect_new(int x, int y, int w, int h){
	SDL_Rect rect = {x, y, w, h};
	return rect;
}

//Label function

Label* Label_new(const char* path, SDL_Surface* parent, SDL_Rect crop, SDL_Rect pos){
	Label* label = (Label*)malloc(sizeof(Label));
	if (!label){
		return NULL;
	}
	label->crop = crop;
	label->pos = pos;
	label->image = loadImage(path);
	if (!label->image){
		free(label);
		return NULL;
	}
	label->parent = parent;
	return label;
}

int Label_draw(void* data){
	Label* label = (Label*)data;	
	if (drawSubImage(label->image, label->crop, label->parent, label->pos)){
		return 1;
	}	
	return 0;
}

void Label_free(void* data){
	Label* label = (Label*)data;
	SDL_FreeSurface(label->image);
	free(label);
}

//Button functions

static Button* Button_new(int id, SDL_Surface* parent, SDL_Rect rect, int y, const char* path){
	Button* button = (Button*)malloc(sizeof(Button));
	if (!button){
		return NULL;
	}
		
	button->img = loadImage(path);
	if (!button->img){
		free(button);
		return NULL;
	}
	button->id = id;
	button->parent = parent;
	button->rect = rect;	
	button->pressed = Rect_new(0, y, rect.w, rect.h);
	button->hovered = Rect_new(rect.w, y, rect.w, rect.h);
	button->normal = Rect_new(2*rect.w, y, rect.w, rect.h);
	button->current = button->normal;
	
	return button;
}

void Button_setToNormal(Button* button){
	button->current = button->normal;
}

void Button_setToHovered(Button* button){
	button->current = button->hovered;
}

void Button_setToPressed(Button* button){
	button->current = button->pressed;
}

static int Button_draw(void* data){
	Button* button = (Button*)data;
	if (drawSubImage(button->img, button->current, button->parent, button->rect)){
		return 1;
	}
	return 0;
}

static void Button_free(void* data){
	Button* button = (Button*) data;
	SDL_FreeSurface(button->img);
	free(button);
}

//radio functions

Radio* Radio_new(const char* path, SDL_Surface* parent, SDL_Rect crop, SDL_Rect pos, int value){
	Radio* radio = (Radio*)malloc(sizeof(Radio));
	if (!radio){
		return NULL;
	}
	radio->value = value;
	radio->pos = pos;
	radio->state = 0;
	radio->label = Label_new(path, parent, crop, pos);
	if (!radio->label){
		free(radio);
		return NULL;
	}
	radio->group = NULL;
	LinkedList_add(window->radios, radio);
	return radio;
}

int Radio_draw(Radio* radio){
	SDL_Surface* radioImg = loadImage("radio.bmp");
	if (!radioImg){
		return 1;
	}
	SDL_Rect radioImgPos = {radio->label->pos.x-24, radio->label->pos.y, 24, 24};
	SDL_Rect radioImgCrop = {0, radio->state*24, 24, 24};
	if (drawSubImage(radioImg, radioImgCrop, radio->label->parent, radioImgPos)){
		SDL_FreeSurface(radioImg);
		return 1;
	}
	SDL_FreeSurface(radioImg);
	if (Label_draw(radio->label)){
		return 1;
	}
	return 0;
}

void Radio_select(Radio* radio, int state){
	Iterator iterator;
	Iterator_init(&iterator, radio->group->radios);
	while(Iterator_hasNext(&iterator)){
		Radio* current = (Radio*)Iterator_next(&iterator);
		current->state = 0;
	}
	radio->state = 1;
	radio->group->selected = radio;
}

void Radio_free(void* data){
	Radio* radio = (Radio*)data;
	Label_free(radio->label);
}

RadioGroup* RadioGroup_new(){
	RadioGroup* group = (RadioGroup*)malloc(sizeof(RadioGroup));
	if (!group){
		return NULL;
	}
	group->radios = LinkedList_new(&Radio_free);
	if (!group->radios){
		free(group);
		return NULL;
	}
	group->selected = NULL;
	return group;
}

void RadioGroup_add(RadioGroup* group, Radio* radio){
	LinkedList_add(group->radios, radio);
	radio->group = group;
}

int RadioGroup_getValue(RadioGroup* group){
	if (!group->selected){
		return 0;
	}
	return group->selected->value;
}

void RadioGroup_free(void* data){
	RadioGroup* group = (RadioGroup*)data;
	LinkedList_free(group->radios);
}

//Panel functions

static Panel* Panel_new(SDL_Surface* parent, SDL_Rect rect, int(*drawFunc)(Panel*)){
	Panel* panel = (Panel*)malloc(sizeof(Panel));
	if (!panel){
		return NULL;
	}
	panel->surface = SDL_CreateRGBSurface(SDL_HWSURFACE, (&rect)->w, (&rect)->h, 32, 0, 0, 0, 0);
	if (!panel->surface){
		free(panel);
		return NULL;
	}
	panel->rect = rect;
	panel->parent = parent;	
	panel->drawFunc = drawFunc;
	panel->children = NULL;
	return panel;
}

static int Panel_flipAndDraw(Panel* panel){
	if (flipBuffer(panel->surface)) {
		return 1;
	}
	if (drawImage(panel->surface, panel->parent, panel->rect.x, panel->rect.y)) {
		return 1;
	}
	return 0;
}

static int MainMenu_draw(Panel* panel){
	drawImageByPath("main.bmp", panel->surface, 0, 0);
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		if (Button_draw(Iterator_next(&iterator))){
			return 1;
		}
	}
	
	if (Panel_flipAndDraw(panel)){
		return 1;
	}
	return 0;
}

static int BoardPanel_draw(Panel* panel){
	SDL_Surface* img = loadImage("pieces.bmp");
	if (!img){
		return 1;
	}

	/* paint pieces */
	for (int x = 1; x <= 8; x++){
		for (int y = 1; y <= 8; y++){
			SDL_Rect crop = {0, 0, TILE_SIZE, TILE_SIZE};
			SDL_Rect piecePosition = {(x-1)*TILE_SIZE, (8-y)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
			
			int isBlackSquare = ((x+y) % 2 == 0);
			if (SDL_FillRect(panel->surface, &piecePosition, isBlackSquare? BLACK_TILE_COLOR : WHITE_TILE_COLOR) != 0){
				printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
				return 1;
			}
			char piece = Board_getPiece(&board, x, y);
			
			switch (piece) {
				case (Board_EMPTY): continue;
				case (Board_WHITE_KING): break;
				case (Board_BLACK_KING):   crop.x = 1*TILE_SIZE; break;
				case (Board_WHITE_QUEEN):  crop.x = 2*TILE_SIZE; break;
				case (Board_BLACK_QUEEN):  crop.x = 3*TILE_SIZE; break;
				case (Board_WHITE_ROOK):   crop.y = 2*TILE_SIZE; break;
				case (Board_BLACK_ROOK):   crop.x = 1*TILE_SIZE; crop.y = 2*TILE_SIZE; break;
				case (Board_WHITE_KNIGHT): crop.y = 1*TILE_SIZE; break;
				case (Board_BLACK_KNIGHT): crop.x = 1*TILE_SIZE; crop.y = 1*TILE_SIZE; break;
				case (Board_WHITE_BISHOP): crop.x = 2*TILE_SIZE; crop.y = 2*TILE_SIZE; break;
				case (Board_BLACK_BISHOP): crop.x = 3*TILE_SIZE; crop.y = 2*TILE_SIZE; break;
				case (Board_WHITE_PAWN):   crop.x = 2*TILE_SIZE; crop.y = 1*TILE_SIZE; break;
				case (Board_BLACK_PAWN):   crop.x = 3*TILE_SIZE; crop.y = 1*TILE_SIZE; break;
			}

			if (drawSubImage(img, crop, panel->surface, piecePosition)){
				return 1;
			}
		}
	}
	// paint moves
	if (selectedX != 0){
		SDL_Rect fromRect = {(selectedX-1)*TILE_SIZE, (8-selectedY)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_Rect selected = {0, 3*TILE_SIZE, TILE_SIZE, TILE_SIZE};
		if (drawSubImage(img, selected, panel->surface, fromRect)){
			return 1;
		}
	}	
	if (movesOfSelectedPiece){		
		Iterator iterator;
		Iterator_init(&iterator, movesOfSelectedPiece);	
		SDL_Rect dest = {TILE_SIZE, 3*TILE_SIZE, TILE_SIZE, TILE_SIZE};
		while (Iterator_hasNext(&iterator)){
			PossibleMove* move = (PossibleMove*)Iterator_next(&iterator);
			SDL_Rect toRect = {((move->toX)-1)*TILE_SIZE, (8-move->toY)*TILE_SIZE, TILE_SIZE, TILE_SIZE};			
			//castle, rook is selected
			if (move->toX == 0){
				int player = Board_getColor(&board, selectedX, selectedY);
				toRect.x = ((board.kingX[player])-1)*TILE_SIZE;
				toRect.y = (8-board.kingY[player])*TILE_SIZE;
			}
			//castle, king is selected
			if (move->fromX != selectedX || move->fromY != selectedY){ 
				toRect.x = ((move->fromX)-1)*TILE_SIZE;
				toRect.y = (8-move->fromY)*TILE_SIZE;
			}
			
			if (drawSubImage(img, dest, panel->surface, toRect)){
				return 1;
			}
		}
	}
	
	if (Panel_flipAndDraw(panel)){
		return 1;
	}
	SDL_FreeSurface(img);
	
	return 0;
}

static int announcementsPanel_draw(Panel* panel){	
	if (fillBackground(panel)){
		return 1;
	}

	if (drawImageByPath("board_letters.bmp", panel->surface, 2*TILE_SIZE, 0) != 0){
		return 1;
	}
		
	if (isInCheck){
		if (drawImageByPath("check.bmp", panel->surface, 5.2*TILE_SIZE, 0.5*TILE_SIZE) != 0){
			return 1;
		}
	}
	
	if(gameEnded){
		if(isInCheck){
			SDL_Surface* mate = loadImage("mate.bmp");
			if(!mate){
				return 1;
			}
			
			int mateImageRowToDrawByTurn = (turn == WHITE)? 25 : 0;
			SDL_Rect crop = {0, mateImageRowToDrawByTurn, mate->w, 0.5*(mate->h)};
			SDL_Rect pos = {3*TILE_SIZE, TILE_SIZE, mate->w, 0.5*(mate->h)};
			
			if (drawSubImage(mate, crop, panel->surface, pos) != 0){
				SDL_FreeSurface(mate);
				return 1;
			}
			
			SDL_FreeSurface(mate);
		}
		
		else{
			if(drawImageByPath("tie.bmp", panel->surface, 3*TILE_SIZE, TILE_SIZE) != 0){
				return 1;
			}
		}
	}
	
	else{
		SDL_Surface* makeYourMove = loadImage("makeYourMove.bmp");
		if (!makeYourMove){
			return 1;
		}
		
		int moveImageRowToDrawByTurn = (turn == WHITE)? 0 : 30;
		SDL_Rect crop = {0, moveImageRowToDrawByTurn, makeYourMove->w, 0.5*(makeYourMove->h)};
		SDL_Rect pos = {3*TILE_SIZE, TILE_SIZE, makeYourMove->w, 0.5*(makeYourMove->h)};
		
		if (drawSubImage(makeYourMove, crop, panel->surface, pos) != 0){
			SDL_FreeSurface(makeYourMove);
			return 1;
		}
		
		SDL_FreeSurface(makeYourMove);
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int boardNumbersPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	if (drawImageByPath("board_numbers.bmp", panel->surface, TILE_SIZE, 0) != 0){
		return 1;
	}
	
	if (drawImageByPath("board_numbers.bmp", panel->surface, 9.6*TILE_SIZE, 0) != 0){
		return 1;
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int buttonsPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	if (drawImageByPath("board_letters.bmp", panel->surface, 2*TILE_SIZE, 1.45*TILE_SIZE) != 0){
		return 1;
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int AISettingsPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	if(drawImageByPath("AISettingsHeader.bmp", panel->surface, 3.75*TILE_SIZE, 0)){
		return 1;
	}
	
	if(drawImageByPath("difficultyHeader.bmp", panel->surface, 1.5*TILE_SIZE, 4.3*TILE_SIZE)){
		return 1;
	}
	
	if(drawImageByPath("AIColorHeader.bmp", panel->surface, 8*TILE_SIZE, 4.3*TILE_SIZE)){
		return 1;
	}
	
	Iterator iterator;
	Iterator_init(&iterator, window->radios);
	while(Iterator_hasNext(&iterator)){
		Radio* radio = (Radio*)Iterator_next(&iterator);
		if (Radio_draw(radio)){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

static void Panel_free(void* data){
	Panel* panel = (Panel*)data;
	SDL_FreeSurface(panel->surface);
	if (panel->children){
		LinkedList_free(panel->children);
	}	
	free(panel);
}

//Window functions

static Window* Window_new(int w, int h){
	Window* window = (Window*)malloc(sizeof(Window));
	if(!window){
		return NULL;
	}
	window->surface = SDL_SetVideoMode(w, h, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (!window->surface) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return NULL;
	}
	window->children = LinkedList_new(&Panel_free);
	if(!window->children){
		return NULL;
	}
	window->buttons = LinkedList_new(&Button_free);
	if(!window->buttons){
		LinkedList_free(window->children);
		return NULL;
	}
	
	window->radios = LinkedList_new(&Radio_free);
	if(!window->radios){
		LinkedList_free(window->children);
		return NULL;
	}
	
	return window;
}
void prepareWindowForNewScreen(){
	LinkedList_free(window->children);
	window->children = LinkedList_new(&Panel_free);
	LinkedList_removeAll(window->buttons);
	LinkedList_removeAll(window->radios);
}

int setScreenToMainMenu(){
	prepareWindowForNewScreen();
	
	SDL_Rect rect = {0, 0, 768, 768};
	Panel* mainMenuPanel = Panel_new(window->surface, rect, &MainMenu_draw);
	if(!mainMenuPanel){
		return 1;
	}
	mainMenuPanel->children = LinkedList_new(&Button_free);
	LinkedList_add(window->children, mainMenuPanel);
	
	SDL_Rect newGameRect = {166, 256, 436, 90};
	Button* newGameButton = Button_new(NEW, mainMenuPanel->surface, newGameRect, 0, "main buttons.bmp");
	LinkedList_add(mainMenuPanel->children, newGameButton);
	LinkedList_add(window->buttons, newGameButton);
	
	SDL_Rect loadGameRect = {166, 346, 436, 90};
	Button* loadGameButton = Button_new(LOAD, mainMenuPanel->surface, loadGameRect, 90, "main buttons.bmp");
	LinkedList_add(mainMenuPanel->children, loadGameButton);
	LinkedList_add(window->buttons, loadGameButton);
	
	SDL_Rect quitRect = {166, 436, 436, 90};
	Button* quitButton = Button_new(QUIT, mainMenuPanel->surface, quitRect, 180, "main buttons.bmp");
	LinkedList_add(mainMenuPanel->children, quitButton);
	LinkedList_add(window->buttons, quitButton);
	
	return 0;
}

int setScreenToGame(){
	prepareWindowForNewScreen();
	
	SDL_Rect announcementsRect = {0, 10*TILE_SIZE, 12*TILE_SIZE, 2*TILE_SIZE};
	Panel* announcementsPanel = Panel_new(window->surface, announcementsRect, &announcementsPanel_draw);
	if(!announcementsPanel){
		return 1;
	}
	
	SDL_Rect boardRect = {2*TILE_SIZE, 2*TILE_SIZE, 8*TILE_SIZE, 8*TILE_SIZE};
	Panel* boardPanel = Panel_new(window->surface, boardRect, &BoardPanel_draw);
	if(!boardPanel){
		return 1;
	}
	
	SDL_Rect boardNumbersRect = {0, 2*TILE_SIZE, 12*TILE_SIZE, 8*TILE_SIZE};
	Panel* boardNumbersPanel = Panel_new(window->surface, boardNumbersRect, &boardNumbersPanel_draw);
	if (!boardNumbersPanel){
		return 1;
	}
	
	SDL_Rect buttonsRect = {0, 0, 12*TILE_SIZE, 2*TILE_SIZE};
	Panel* buttonsPanel = Panel_new(window->surface, buttonsRect, &buttonsPanel_draw);
	if(!buttonsPanel){
		return 1;
	}
	
	LinkedList_add(window->children, boardNumbersPanel);
	LinkedList_add(window->children, buttonsPanel);
	LinkedList_add(window->children, boardPanel);
	LinkedList_add(window->children, announcementsPanel);
	return 0;
}

int setScreenToAISettings(){
	prepareWindowForNewScreen();
	
	SDL_Rect AISettingsRect = {0, 0, 12*TILE_SIZE, 12*TILE_SIZE};
	
	Panel* AISettingsPanel = Panel_new(window->surface, AISettingsRect, &AISettingsPanel_draw);
	if (!AISettingsPanel){
		return 1;
	}
	
	RadioGroup* difficultyRadioGroup = RadioGroup_new();
	if (!difficultyRadioGroup){
		return 1;
	}
	
	for (int i = 0; i <= 4; i++){
		SDL_Rect crop = {0, i*24, 48, 24};
		SDL_Rect pos = {24+3*TILE_SIZE, i*24+5*TILE_SIZE, 48, 24};
		Radio* difficultyRadio = Radio_new("difficultyLabels.bmp", AISettingsPanel->surface, crop, pos, i);
		if (!difficultyRadio){
			RadioGroup_free(difficultyRadioGroup);
			return 1;
		}
		RadioGroup_add(difficultyRadioGroup, difficultyRadio);
		if(Radio_draw(difficultyRadio)){
			RadioGroup_free(difficultyRadioGroup);
			return 1;
		}
	}
	
	RadioGroup* AIColorRadioGroup = RadioGroup_new();
	if (!AIColorRadioGroup){
		return 1;
	}
	
	for (int i = 0; i <= 1; i++){
		SDL_Rect crop = {0, i*24, 48, 24};
		SDL_Rect pos = {24+8.5*TILE_SIZE, i*24+5*TILE_SIZE, 48, 24};
		Radio* AIColorRadio = Radio_new("AIColorLabels.bmp", AISettingsPanel->surface, crop, pos, i);
		if (!AIColorRadio){
			RadioGroup_free(difficultyRadioGroup);
			RadioGroup_free(AIColorRadioGroup);
			return 1;
		}
		RadioGroup_add(AIColorRadioGroup, AIColorRadio);
		if(Radio_draw(AIColorRadio)){
			RadioGroup_free(difficultyRadioGroup);
			RadioGroup_free(AIColorRadioGroup);
			return 1;
		}
	}
	
	LinkedList_add(window->children, AISettingsPanel);
	return 0;
}

static void Window_free(){
	LinkedList_free(window->children);
	if (movesOfSelectedPiece){
		LinkedList_free(movesOfSelectedPiece);
	}
	LinkedList_removeAll(window->buttons);
	free(window->buttons);
	free(window);
	SDL_Quit();
}

//GUI functions
	
int GUI_init(){
	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(Window_free);
	
	//create tree
	window = Window_new(768, 768);
	if(!window){
		return 1;
	}
	setScreenToMainMenu();
	return 0;
}

int GUI_paint(){
	// Clear window to BLACK
	if (SDL_FillRect(window->surface, 0, BACKGROUND_WHITE) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return 1;
	}
	
	//draw all panels
	Iterator iterator;
	Iterator_init(&iterator, window->children);
	while(Iterator_hasNext(&iterator)){
		Panel* panel = (Panel*)Iterator_next(&iterator);
		panel->drawFunc(panel);
	}
	
	// We finished drawing
	if (flipBuffer(window->surface)) {
		return 1;
	}
	return 0;
}

static int Rect_contains(SDL_Rect rect, int x, int y){
	return x >= rect.x && x < rect.x+rect.w && y >= rect.y && y < rect.y+rect.h;
}

Button* getButtonByMousePosition(int x, int y){
	Iterator iterator;
	Iterator_init(&iterator, window->buttons);
	while (Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		if (Rect_contains(button->rect, x, y)){
			return button;
		}
	}
	return NULL;
}

Radio* getRadioByMousePosition(int x, int y){
	Iterator iterator;
	Iterator_init(&iterator, window->radios);
	while (Iterator_hasNext(&iterator)){
		Radio* radio = (Radio*)Iterator_next(&iterator);
		if (Rect_contains(radio->pos, x+24, y)){
			return radio;
		}
	}
	return NULL;
}

void setAllButtonsToNormal(){
	Iterator iterator;
	Iterator_init(&iterator, window->buttons);
	while (Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		Button_setToNormal(button);
	}
}

// int PromotionDialog_draw(void* data){
	// int error = 0;
	// Panel* promotionDialog = (Panel*)data;

	// promotionDialog->children = LinkedList_new(&Button_free);
	// if(!promotionDialog->children){
		// return 1;
	// }
	
	// char* possiblePromotions = (turn == BLACK)? (char[4]){Board_BLACK_QUEEN, Board_BLACK_BISHOP, Board_BLACK_ROOK, Board_BLACK_KNIGHT}:
											      // (char[4]){Board_WHITE_QUEEN, Board_WHITE_BISHOP, Board_WHITE_ROOK, Board_WHITE_KNIGHT};
	// //creating the buttons
	// for (int x = 1; x <= 2; x++){
		// for (int y = 1; y <= 2; y++){
			// int id = (x-1)*2 + y;
			// int row;
			// char piece = possiblePromotions[(x-1)*2 + (y-1)];
			// SDL_Rect piecePosition = {(x-1)*TILE_SIZE, (2-y)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
			// switch (piece) {
				// case (Board_WHITE_QUEEN):
				// case (Board_BLACK_QUEEN): row = 0; break;
				// case (Board_WHITE_ROOK): 
				// case (Board_BLACK_ROOK): row = 128; break;
				// case (Board_WHITE_KNIGHT):
				// case (Board_BLACK_KNIGHT): row = 192; break;
				// case (Board_WHITE_BISHOP):
				// case (Board_BLACK_BISHOP): row = 64; break;
			// }
			
			// Button* button = (turn == BLACK)? Button_new(id, promotionDialog, piecePosition, row, "blackPromotionButtons.bmp"):
											  // Button_new(id, promotionDialog, piecePosition, row, "whitePromotionButtons.bmp");
			// LinkedList_add(promotionDialog->children, button);
		// }
	// }

	// //drawing the buttons
	
	// Iterator iterator;
	// Iterator_init(&iterator, promotionDialog->children);
	// while (Iterator_hasNext(&iterator)){
		// Button* button = (Button*)Iterator_next(&iterator);
		// error = Button_draw(button);
	// }
	
	// return error;
// }