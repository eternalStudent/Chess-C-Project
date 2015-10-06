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

static SDL_Rect findAbsoluteRectPosition(SDL_Rect rect, Panel* parent){
	int panelX = parent->rect.x;
	int panelY = parent->rect.y;
	int rectX = rect.x;
	int rectY = rect.y;
	
	return Rect_new(panelX+rectX, panelY+rectY, rect.w, rect.h);
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

static Button* Button_new(int id, Panel* parent, SDL_Rect rect, int y, const char* path){
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
	button->hidden = 0;
	button->parent = parent->surface;
	button->relativeRect = rect;
	SDL_Rect absoluteRect = findAbsoluteRectPosition(rect, parent);
	button->absoluteRect = absoluteRect;	
	button->pressed = Rect_new(0, y, rect.w, rect.h);
	button->hovered = Rect_new(rect.w, y, rect.w, rect.h);
	button->normal = Rect_new(2*rect.w, y, rect.w, rect.h);
	button->current = button->normal;
	
	if (LinkedList_add(window->buttons, button) || LinkedList_add(parent->children, button)){
		return NULL;
	}
	
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

void Button_hide(Button* button){
	button->hidden = 1;
}

static int Button_draw(void* data){
	Button* button = (Button*)data;
	if (button->hidden){
		return 0;
	}
	if (drawSubImage(button->img, button->current, button->parent, button->relativeRect)){
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

Radio* Radio_new(const char* path, Panel* parent, SDL_Rect crop, SDL_Rect pos, int value){
	Radio* radio = (Radio*)malloc(sizeof(Radio));
	if (!radio){
		return NULL;
	}
	radio->value = value;
	radio->pos = pos;
	radio->state = 0;
	radio->label = Label_new(path, parent->surface, crop, pos);
	if (!radio->label){
		free(radio);
		return NULL;
	}
	radio->absolutePos = findAbsoluteRectPosition(pos, parent);
	radio->group = NULL;
	radio->hidden = 0;
	LinkedList_add(window->radios, radio);
	return radio;
}

int Radio_draw(Radio* radio){
	SDL_Surface* radioImg = loadImage("Textures/radio.bmp");
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
	int* adjustedParameter = radio->group->parameter;
	*adjustedParameter = radio->value; 
}

void Radio_free(void* data){
	Radio* radio = (Radio*)data;
	Label_free(radio->label);
	free(radio);
}

void RadioGroup_changeVisibility(RadioGroup* group, short hide){
	group->hidden = hide;
	Iterator iterator;
	Iterator_init(&iterator, group->radios);
	while(Iterator_hasNext(&iterator)){
		Radio* radio = (Radio*)Iterator_next(&iterator);
		radio->hidden = hide;
	}
}


RadioGroup* RadioGroup_new(int* parameter){
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
	group->hidden = 0;
	group->parameter = parameter;
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

int RadioGroup_draw(RadioGroup* group){
	if (group->hidden){
		return 0;
	}
	Iterator iterator;
	Iterator_init(&iterator, group->radios);
	while(Iterator_hasNext(&iterator)){
		Radio* radio = (Radio*)Iterator_next(&iterator);
		if(Radio_draw(radio)){
			return 1;
		}
	}
	return 0;
}

void RadioGroup_free(void* data){
	RadioGroup* group = (RadioGroup*)data;
	LinkedList_free(group->radios);
	free(group);
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
	panel->hidden = 0;
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
	drawImageByPath("Textures/main.bmp", panel->surface, 0, 0);
	
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

static int drawBoard(Panel* panel, Board* board){
	SDL_Surface* img = loadImage("Textures/pieces.bmp");
	if (!img){
		SDL_FreeSurface(img);
		return 1;
	}
	
	for (int x = 1; x <= 8; x++){
		for (int y = 1; y <= 8; y++){
			SDL_Rect crop = {0, 0, TILE_SIZE, TILE_SIZE};
			SDL_Rect piecePosition = {(x-1)*TILE_SIZE, (8-y)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
			
			int isBlackSquare = ((x+y) % 2 == 0);
			if (SDL_FillRect(panel->surface, &piecePosition, isBlackSquare? BLACK_TILE_COLOR : WHITE_TILE_COLOR) != 0){
				printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
				SDL_FreeSurface(img);
				return 1;
			}
			char piece = Board_getPiece(board, x, y);
			
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
				SDL_FreeSurface(img);
				return 1;
			}
		}
	}
	if (Panel_flipAndDraw(panel)){
		SDL_FreeSurface(img);
		return 1;
	}
	
	SDL_FreeSurface(img);
	
	return 0;
}

static int piecesPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
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

static int settingsBoardPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	if (drawBoard(panel, copyOfMainBoard)){
		return 1;
	}
		
	if (Panel_flipAndDraw(panel)){
		return 1;
	}
	
	return 0;
	
}

static int boardSettingsHeaderPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	if (drawImageByPath("Textures/boardSettingsHeader.bmp", panel->surface, 4.2*TILE_SIZE, 0.2*TILE_SIZE)){
		return 1;
	}
	
	if (drawImageByPath("Textures/boardSettingsInstructions.bmp", panel->surface, 1.2*TILE_SIZE ,0.7*TILE_SIZE)){
		return 1;
	}
	
	if (settingInvalidPiece){
		if (drawImageByPath("Textures/settingPieceError.bmp", panel->surface, 1.7*TILE_SIZE, 1.2*TILE_SIZE)){
			return 1;
		}
	}
	
	if (kingIsMissing){
		if (drawImageByPath("Textures/kingMissingError.bmp", panel->surface, 3.2*TILE_SIZE, 1.2*TILE_SIZE)){
			return 1;
		}
	}
	
	if (Panel_flipAndDraw(panel)){
		return 1;
	}
	
	return 0;
	
}

static int gameBoardPanel_draw(Panel* panel){
	SDL_Surface* img = loadImage("Textures/pieces.bmp");
	if (!img){
		SDL_FreeSurface(img);
		return 1;
	}
	
	if(drawBoard(panel, &board)){
		SDL_FreeSurface(img);
		return 1;
	}
	
	// paint moves
	if (selectedX != 0){
		SDL_Rect fromRect = {(selectedX-1)*TILE_SIZE, (8-selectedY)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
		SDL_Rect selected = {0, 3*TILE_SIZE, TILE_SIZE, TILE_SIZE};
		if (drawSubImage(img, selected, panel->surface, fromRect)){
			SDL_FreeSurface(img);
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
				SDL_FreeSurface(img);
				return 1;
			}
		}
	}
	
	if (Panel_flipAndDraw(panel)){
		SDL_FreeSurface(img);
		return 1;
	}
	SDL_FreeSurface(img);
	
	return 0;
}

static int announcementsPanel_draw(Panel* panel){	
	if (fillBackground(panel)){
		return 1;
	}

	if (drawImageByPath("Textures/board_letters.bmp", panel->surface, 2*TILE_SIZE, 0) != 0){
		return 1;
	}
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		if (Button_draw(Iterator_next(&iterator))){
			return 1;
		}
	}
		
	if (isInCheck && !gameEnded){
		if (drawImageByPath("Textures/check.bmp", panel->surface, 5.2*TILE_SIZE, 0.5*TILE_SIZE) != 0){
			return 1;
		}
	}
	
	if(gameEnded){
		if(isInCheck){
			SDL_Surface* mate = loadImage("Textures/mate.bmp");
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
			if(drawImageByPath("Textures/tie.bmp", panel->surface, 3*TILE_SIZE, TILE_SIZE) != 0){
				return 1;
			}
		}
	}
	
	else{
		SDL_Surface* makeYourMove = loadImage("Textures/makeYourMove.bmp");
		if (!makeYourMove){
			return 1;
		}
		
		int moveImageRowToDrawByTurn = (turn == WHITE)? 0 : 30;
		int promotionImageRowToDrawByTurn = (turn == WHITE)? 60 : 90;
		int rowToDraw;
		
		if ((turn != player1) && (gameMode == SINGLE_PLAYER_MODE)){
			rowToDraw = (turn == WHITE)? 120 : 150;
		}
		
		else{ 
			rowToDraw = (chosePromotionMove)? promotionImageRowToDrawByTurn : moveImageRowToDrawByTurn; 
		}
 
		SDL_Rect crop = {0, rowToDraw, 375, 30};
		SDL_Rect pos = {3*TILE_SIZE, TILE_SIZE, 375, 30};
		
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
	
	if (drawImageByPath("Textures/board_numbers.bmp", panel->surface, TILE_SIZE, 0) != 0){
		return 1;
	}
	
	if (drawImageByPath("Textures/board_numbers.bmp", panel->surface, 9.6*TILE_SIZE, 0) != 0){
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
	
	if (drawImageByPath("Textures/board_letters.bmp", panel->surface, 2*TILE_SIZE, 1.45*TILE_SIZE) != 0){
		return 1;
	}
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		if (Button_draw(Iterator_next(&iterator))){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int AISettingsHeaderPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	if(drawImageByPath("Textures/AISettingsHeader.bmp", panel->surface, 3.75*TILE_SIZE, 0)){
		return 1;
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}


int AISettingsRadiosPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	if(drawImageByPath("Textures/difficultyHeader.bmp", panel->surface, 1.5*TILE_SIZE, 0)){
		return 1;
	}
	
	if(drawImageByPath("Textures/AIColorHeader.bmp", panel->surface, 8*TILE_SIZE, 0)){
		return 1;
	}
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		RadioGroup* radioGroup = (RadioGroup*)Iterator_next(&iterator);
		if (RadioGroup_draw(radioGroup)){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int AISettingsButtonsPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		if (Button_draw(button)){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int instructionsPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}
	if (drawImageByPath("Textures/instructions.bmp", panel->surface, 1.5*TILE_SIZE, 0.5*TILE_SIZE) != 0){
		return 1;
	}
		
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		if (Button_draw(button)){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int playerSettingsHeader_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}	
	
	if (drawImageByPath("Textures/playerSettingsIcon.bmp", panel->surface, 4.2*TILE_SIZE, 2*TILE_SIZE)){
		return 1;
	}
	
	if(drawImageByPath("Textures/playerSettingsHeader.bmp", panel->surface, 3*TILE_SIZE ,0.5*TILE_SIZE) != 0){
		return 1;
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int playerSettingsRadiosPanel_draw (Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}	
	
	if (drawImageByPath("Textures/gameModeHeader.bmp", panel->surface, 1.8*TILE_SIZE, 0)){
		return 1;
	}
	
	if (drawImageByPath("Textures/nextPlayerHeader.bmp", panel->surface, 7.8*TILE_SIZE, 0)){
		return 1;
	}
	
	if (gameMode == TWO_PLAYERS_MODE){
		if(drawImageByPath("Textures/depthForHintsHeader.bmp", panel->surface, 3.7*TILE_SIZE, 1.8*TILE_SIZE)){
			return 1;
		}
		RadioGroup* depthForHintsRadioGroup = LinkedList_last(panel->children);
		RadioGroup_changeVisibility(depthForHintsRadioGroup, 0);
	}
	
	if (gameMode == SINGLE_PLAYER_MODE){
		RadioGroup* depthForHintsRadioGroup = LinkedList_last(panel->children);
		RadioGroup_changeVisibility(depthForHintsRadioGroup, 1);
	}
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		RadioGroup* radioGroup = (RadioGroup*)Iterator_next(&iterator);
		if (RadioGroup_draw(radioGroup)){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}


int playerSettingsButtonsPanel_draw(Panel* panel){
	if (fillBackground(panel)){
		return 1;
	}	
	
	Button* AISettingsButton = LinkedList_first(panel->children);
	Button* letsPlayButton = LinkedList_second(panel->children);
	
	if (gameMode == SINGLE_PLAYER_MODE){
		AISettingsButton->hidden = 0;
		letsPlayButton->hidden = 1;
	}
	else{
		AISettingsButton->hidden = 1;
		letsPlayButton->hidden = 0;
	}
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		if (Button_draw(button)){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int saveLoadPanel_draw(Panel* panel, short save){
	if (fillBackground(panel)){
		return 1;
	}
	
	SDL_Surface* image = loadImage("Textures/saveLoad.bmp");
	if (!image){
		return 1;
	}
	SDL_Rect crop = {0, save*60, 264, 60};
	SDL_Rect pos = {252, 32, 264, 60};
	if (drawSubImage(image, crop, panel->surface, pos)){
		return 1;
	}
	SDL_FreeSurface(image);
	
	if (LinkedList_length(panel->children) <= 1){
		if (drawImageByPath("Textures/noSaveSlots.bmp", panel->surface, 207, 256)){
			return 1;
		}
	}
	
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		if (Button_draw(button)){
			return 1;
		}
	}
	
	if(Panel_flipAndDraw(panel) != 0){
		return 1;
	}
	
	return 0;
}

int loadPanel_draw(Panel* panel){
	return saveLoadPanel_draw(panel, 0);
}

int savePanel_draw(Panel* panel){
	return saveLoadPanel_draw(panel, 1);
}

int promotionPanel_draw(Panel* panel){
	panel->hidden = (chosePromotionMove)? 0 : 1;

	if (panel->hidden){
		Iterator iterator;
		Iterator_init(&iterator, panel->children);
		while(Iterator_hasNext(&iterator)){
			Button* button = (Button*)Iterator_next(&iterator);
			button->hidden = 1;
		}
		return 0;
	}
	
	if (fillBackground(panel)){
		return 1;
	}
	
	int index = 0;
	Iterator iterator;
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		if ((turn == WHITE && index < 4) || (turn == BLACK && index > 3)){
			// printf("index is %d, turn is %d, so turning this button's hidden to 1\n", index, turn);
			button->hidden = 1;
		}
		if ((turn == WHITE && index > 3) || (turn == BLACK && index < 4)){
			// printf("index is %d, turn is %d, so turning this button's hidden to 0\n", index, turn);
			button->hidden = 0;
		}
		index++;
	}
	
	Iterator_init(&iterator, panel->children);
	while(Iterator_hasNext(&iterator)){
		Button* button = (Button*)Iterator_next(&iterator);
		if (Button_draw(button)){
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
	LinkedList_removeAllAndFree(window->children);
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
	LinkedList_add(window->children, mainMenuPanel);
	
	mainMenuPanel->children = LinkedList_new(&Button_free);
	if(!mainMenuPanel->children){
		return 1;
	}
	int buttonsId[3] = {NEW, LOAD, QUIT};
	for (int y = 0; y <= 180; y += 90){
		SDL_Rect pos = {166, 256+y, 436, 90};
		Button* button = Button_new(buttonsId[y/90], mainMenuPanel, pos, y, "Textures/main buttons.bmp");
		if (!button){
			return 1;
		}
	}
	
	return 0;
}

int setScreenToGame(short calledAtBeginningOfGame){
	prepareWindowForNewScreen();
	selectedX = 0;
	
	if (calledAtBeginningOfGame){
		turn = first;
	}

	SDL_Rect announcementsRect = {0, 10*TILE_SIZE, 12*TILE_SIZE, 2*TILE_SIZE};
	Panel* announcementsPanel = Panel_new(window->surface, announcementsRect, &announcementsPanel_draw);
	if(!announcementsPanel){
		return 1;
	}
	
	announcementsPanel->children = LinkedList_new(&Button_free);
	if (!announcementsPanel->children){
		return 1;
	}
	
	SDL_Rect instructionsRect = {10.2*TILE_SIZE, 0.1*TILE_SIZE, 96, 96};
	Button* instructionsButton = Button_new(INSTRUCTIONS, announcementsPanel, instructionsRect, 0, "Textures/instructionsButton.bmp");
	if (!instructionsButton){
		return 1;
	}

	SDL_Rect boardRect = {2*TILE_SIZE, 2*TILE_SIZE, 8*TILE_SIZE, 8*TILE_SIZE};
	Panel* boardPanel = Panel_new(window->surface, boardRect, &gameBoardPanel_draw);
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
	buttonsPanel->children = LinkedList_new(&Button_free);
	if(!buttonsPanel->children){
		return 1;
	}
	
	int buttonValues[] = {MAIN_MENU, SAVE, QUIT, HINT};
	for (int i = 0; i <= 3; i++){
		SDL_Rect buttonRect = {32 + 186*i, 0.2*TILE_SIZE, 146, 40};
		int j = i == 3? 8: i;
		Button* button = Button_new(buttonValues[i], buttonsPanel, buttonRect, j*40, "Textures/gameButtons.bmp");
		if(!button){
			return 1;
		}
	}
	
	SDL_Rect promotionRect = {11*TILE_SIZE, 2*TILE_SIZE, TILE_SIZE, 4*TILE_SIZE};
	Panel* promotionPanel = Panel_new(window->surface, promotionRect, &promotionPanel_draw);
	if (!promotionPanel){
		return 1;
	}
	promotionPanel->children = LinkedList_new(&Button_free);
	if(!promotionPanel->children){
		return 1;
	}
	promotionPanel->hidden = 1;
	
	char* possiblePromotions = (char[8]){Board_BLACK_QUEEN, Board_BLACK_BISHOP, Board_BLACK_ROOK, Board_BLACK_KNIGHT,
							   Board_WHITE_QUEEN, Board_WHITE_BISHOP, Board_WHITE_ROOK, Board_WHITE_KNIGHT};				    
	for (int i = 0; i <= 7; i++){
		char piece = possiblePromotions[i];
		SDL_Rect piecePosition = {0, (i%4)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
		short row, value;
		switch (piece) {
			case (Board_WHITE_QUEEN): value = PROMOTE_TO_WHITE_QUEEN; row = 0; break; 
			case (Board_BLACK_QUEEN): value = PROMOTE_TO_BLACK_QUEEN; row = 0; break;
			case (Board_WHITE_ROOK): value = PROMOTE_TO_WHITE_ROOK; row = 128; break;
			case (Board_BLACK_ROOK): value = PROMOTE_TO_BLACK_ROOK; row = 128; break;
			case (Board_WHITE_KNIGHT): value = PROMOTE_TO_WHITE_KNIGHT; row = 192; break;
			case (Board_BLACK_KNIGHT): value = PROMOTE_TO_BLACK_KNIGHT; row = 192; break;
			case (Board_WHITE_BISHOP): value = PROMOTE_TO_WHITE_BISHOP; row = 64; break;
			case (Board_BLACK_BISHOP): value = PROMOTE_TO_BLACK_BISHOP; row = 64; break;
		}
		Button* button = (i<4)? Button_new(value, promotionPanel, piecePosition, row, "Textures/blackPromotionButtons.bmp"):
							    Button_new(value, promotionPanel, piecePosition, row, "Textures/whitePromotionButtons.bmp");
		if (!button){
			return 1;
		}
		
		if ((turn == WHITE && i<4) || (turn == BLACK && i>3)){
			button->hidden = 1;
		}
	}
	
	LinkedList_add(window->children, boardNumbersPanel);
	LinkedList_add(window->children, buttonsPanel);
	LinkedList_add(window->children, announcementsPanel);
	LinkedList_add(window->children, boardPanel);
	LinkedList_add(window->children, promotionPanel);
	return 0;
}

int setScreenToAISettings(){
	prepareWindowForNewScreen();
	
	SDL_Rect AISettingsHeaderPanelRect = {0, 0, 12*TILE_SIZE, TILE_SIZE}; 
	Panel* AISettingsHeaderPanel = Panel_new(window->surface, AISettingsHeaderPanelRect, &AISettingsHeaderPanel_draw);
	if (!AISettingsHeaderPanel){
		return 1;
	}
	
	SDL_Rect AISettingsRadiosPanelRect = {0, 4.3*TILE_SIZE, 12*TILE_SIZE, 5*TILE_SIZE};
	Panel* AISettingsRadiosPanel = Panel_new(window->surface, AISettingsRadiosPanelRect, &AISettingsRadiosPanel_draw);
	if (!AISettingsRadiosPanel){
		return 1;
	}
	AISettingsRadiosPanel->children = LinkedList_new(&RadioGroup_free);
	if (!AISettingsRadiosPanel->children){
		return 1;
	}
	
	RadioGroup* difficultyRadioGroup = RadioGroup_new(&maxRecursionDepth);
	if (!difficultyRadioGroup){
		return 1;
	}
	
	for (int i = 0; i <= 4; i++){
		SDL_Rect crop = {0, i*24, 48, 24};
		SDL_Rect pos = {24+3*TILE_SIZE, i*24+TILE_SIZE, 48, 24};
		Radio* difficultyRadio = Radio_new("Textures/difficultyLabels.bmp", AISettingsRadiosPanel, crop, pos, i);
		if (!difficultyRadio){
			return 1;
		}
		RadioGroup_add(difficultyRadioGroup, difficultyRadio);
		if (i == maxRecursionDepth){
			difficultyRadio->state = 1;
			difficultyRadio->group->selected = difficultyRadio;
		}
	}
	
	LinkedList_add(AISettingsRadiosPanel->children, difficultyRadioGroup);
	
	RadioGroup* AIColorRadioGroup = RadioGroup_new(&player1);
	if (!AIColorRadioGroup){
		return 1;
	}
	
	for (int i = 0; i <= 1; i++){
		SDL_Rect crop = {0, i*24, 48, 24};
		SDL_Rect pos = {24+8.5*TILE_SIZE, i*24+TILE_SIZE, 48, 24};
		Radio* AIColorRadio = Radio_new("Textures/nextPlayerLabels.bmp", AISettingsRadiosPanel, crop, pos, !i);
		if (!AIColorRadio){
			return 1;
		}
		RadioGroup_add(AIColorRadioGroup, AIColorRadio);
		if (i == !player1){
			AIColorRadio->state = 1;
			AIColorRadio->group->selected = AIColorRadio;
		}	
	}
	
	LinkedList_add(AISettingsRadiosPanel->children, AIColorRadioGroup);
	
	
	SDL_Rect AISettingsButtonsPanelRect = {0, 11*TILE_SIZE, 12*TILE_SIZE, TILE_SIZE};
	Panel* AISettingsButtonsPanel = Panel_new(window->surface, AISettingsButtonsPanelRect, &AISettingsButtonsPanel_draw);
	if (!AISettingsButtonsPanel){
		return 1;
	}
	AISettingsButtonsPanel->children = LinkedList_new(&Button_free);
	if (!AISettingsButtonsPanel->children){
		return 1;
	}
	
	SDL_Rect cancelRect = {2*TILE_SIZE, 0, 146, 40};
	Button* cancelButton = Button_new(RETURN_TO_PLAYER_SETTINGS, AISettingsButtonsPanel, cancelRect, 120, "Textures/gameButtons.bmp");
	if(!cancelButton){
		return 1;
	}
	
	SDL_Rect letsPlayRect = {8*TILE_SIZE, 0, 146, 40};
	Button* letsPlayButton = Button_new(PLAY, AISettingsButtonsPanel, letsPlayRect, 280, "Textures/gameButtons.bmp");
	if (!letsPlayButton){
		return 1;
	}
		
	LinkedList_add(window->children, AISettingsHeaderPanel);
	LinkedList_add(window->children, AISettingsRadiosPanel);
	LinkedList_add(window->children, AISettingsButtonsPanel);
	return 0;
}

int setScreenToInstructions(){
	prepareWindowForNewScreen();
	SDL_Rect instructionsRect = {0, 0, 12*TILE_SIZE, 12*TILE_SIZE};
	Panel* instructionsPanel = Panel_new(window->surface, instructionsRect, &instructionsPanel_draw);
	if (!instructionsPanel){
		return 1;
	}
	instructionsPanel->children = LinkedList_new(&Button_free);
	if (!instructionsPanel->children){
		return 1;
	}
	
	SDL_Rect gotItRect = {5*TILE_SIZE, 10*TILE_SIZE, 146, 40};
	Button* gotItButton = Button_new(RETURN_TO_GAME, instructionsPanel, gotItRect, 360,"Textures/gameButtons.bmp");
	if (!gotItButton){
		return 1;
	}

	LinkedList_add(window->children, instructionsPanel);
	return 0;
}

int setScreenToBoardSettings(){
	prepareWindowForNewScreen();
	
	copyOfMainBoard = Board_new();
	if (!copyOfMainBoard){
		return 1;
	}
	Board_copy(copyOfMainBoard, &board);
	PieceCounter_copy(copyOfMainPieceCounter, counter);
	
	SDL_Rect headerRect = {0, 0, 12*TILE_SIZE, 2*TILE_SIZE};
	Panel* headerPanel = Panel_new(window->surface, headerRect, &boardSettingsHeaderPanel_draw);
	if(!headerPanel){
		return 1;
	}

	SDL_Rect boardRect = {2*TILE_SIZE, 2*TILE_SIZE, 8*TILE_SIZE, 8*TILE_SIZE};
	Panel* boardPanel = Panel_new(window->surface, boardRect, &settingsBoardPanel_draw);
	if(!boardPanel){
		return 1;
	}
	
	SDL_Rect piecesRect = {0, 10*TILE_SIZE, 12*TILE_SIZE, 2*TILE_SIZE};
	Panel* piecesPanel = Panel_new(window->surface, piecesRect, &piecesPanel_draw);
	if(!piecesPanel){
		return 1;
	}
	
	piecesPanel->children = LinkedList_new(&Button_free);
	if(!piecesPanel->children){
		return 1;
	}
	
	//piece buttons
	for (int i = 3; i <= 16; i++){
		SDL_Rect buttonRect = {(((i-((i-3)%2))/2)-1)*TILE_SIZE + 2.5*TILE_SIZE, ((i-3)%2)*TILE_SIZE, TILE_SIZE, TILE_SIZE};
		Button* button = Button_new(i, piecesPanel, buttonRect, (i-3)*TILE_SIZE, "Textures/piecesButtons.bmp");
		if (!button){
			return 1;
		}
	}
	
	SDL_Rect cancelRect = {8, TILE_SIZE, 146, 40};
	Button* cancelButton = Button_new(RETURN_TO_PLAYER_SETTINGS_WITHOUT_SAVING, piecesPanel, cancelRect, 120, "Textures/gameButtons.bmp");
	if(!cancelButton){
		return 1;
	}
	
	SDL_Rect finishedRect = {9.5*TILE_SIZE, TILE_SIZE, 146, 40};
	Button* finishedButton = Button_new(FINISHED_SETTING_BOARD, piecesPanel, finishedRect, 200, "Textures/gameButtons.bmp");
	if(!finishedButton){
		return 1;
	}
	
	LinkedList_add(window->children, headerPanel);
	LinkedList_add(window->children, boardPanel);
	LinkedList_add(window->children, piecesPanel);
	
	return 0;
}


int setScreenToPlayerSettings(){
	prepareWindowForNewScreen();
	
	SDL_Rect playerSettingsRadiosRect = {0, 5*TILE_SIZE, 12*TILE_SIZE, 4.5*TILE_SIZE};
	Panel* playerSettingsRadiosPanel = Panel_new(window->surface, playerSettingsRadiosRect, &playerSettingsRadiosPanel_draw);
	if (!playerSettingsRadiosPanel){
		return 1;
	}
	
	playerSettingsRadiosPanel->children = LinkedList_new(&RadioGroup_free);
	if (!playerSettingsRadiosPanel->children){
		return 1;
	}
	
	RadioGroup* gameModeRadioGroup = RadioGroup_new(&gameMode);
	if (!gameModeRadioGroup){
		return 1;
	}
	
	for (int i = 1; i <= 2; i++){
		SDL_Rect crop = {0, (i-1)*24, 160, 24};
		SDL_Rect pos = {24+2*TILE_SIZE, (i-1)*24+0.5*TILE_SIZE, 160, 24};
		Radio* gameModeRadio = Radio_new("Textures/gameModeLabels.bmp", playerSettingsRadiosPanel, crop, pos, i);
		if (!gameModeRadio){
			RadioGroup_free(gameModeRadioGroup);
			return 1;
		}
		RadioGroup_add(gameModeRadioGroup, gameModeRadio);
		if (i == gameMode){
			gameModeRadio->state = 1;
			gameModeRadio->group->selected = gameModeRadio;
		}
	}
	
	LinkedList_add(playerSettingsRadiosPanel->children, gameModeRadioGroup);

	RadioGroup* nextPlayerRadioGroup = RadioGroup_new(&first);
	if (!nextPlayerRadioGroup){
		return 1;
	}
	
	for (int i = 0; i <= 1; i++){
		SDL_Rect crop = {0, i*24, 60, 24};
		SDL_Rect pos = {24+8.5*TILE_SIZE, i*24+0.5*TILE_SIZE, 60, 24};
		
		Radio* nextPlayerRadio = Radio_new("Textures/nextPlayerLabels.bmp", playerSettingsRadiosPanel, crop, pos, i);
		if (!nextPlayerRadio){
			RadioGroup_free(nextPlayerRadioGroup);
			return 1;
		}
		RadioGroup_add(nextPlayerRadioGroup,nextPlayerRadio);
		if (i == first){
			nextPlayerRadio->state = 1;
			nextPlayerRadio->group->selected = nextPlayerRadio;
		}
	}
	
	LinkedList_add(playerSettingsRadiosPanel->children, nextPlayerRadioGroup);
	
	
	SDL_Rect playerSettingsHeaderRect = {0, 0, 12*TILE_SIZE, 4*TILE_SIZE};
	Panel* playerSettingsHeaderPanel = Panel_new(window->surface, playerSettingsHeaderRect, &playerSettingsHeader_draw);
	if(!playerSettingsHeaderPanel){
		return 1;
	}
	
	SDL_Rect playerSettingsButtonsRect = {0, 10*TILE_SIZE, 12*TILE_SIZE, 2*TILE_SIZE};
	Panel* playerSettingsButtonsPanel = Panel_new(window->surface, playerSettingsButtonsRect, &playerSettingsButtonsPanel_draw);
	if (!playerSettingsButtonsPanel){
		return 1;
	}
	
	playerSettingsButtonsPanel->children = LinkedList_new(&Button_free);
	if(!playerSettingsButtonsPanel->children){
		return 1;
	}
	
	SDL_Rect letsPlayOrAISettings_Rect = {8*TILE_SIZE, TILE_SIZE, 146, 40};
	Button* AISettingsButton = Button_new(AI_SETTINGS, playerSettingsButtonsPanel, letsPlayOrAISettings_Rect, 240, "Textures/gameButtons.bmp");
	if (!AISettingsButton){
		return 1;
	}
	
	Button* letsPlayButton = Button_new(PLAY, playerSettingsButtonsPanel, letsPlayOrAISettings_Rect, 280, "Textures/gameButtons.bmp");
	if (!letsPlayButton){
		return 1;
	}
	
	SDL_Rect cancelRect = {2*TILE_SIZE, TILE_SIZE, 146, 40};
	Button* cancelButton = Button_new(MAIN_MENU, playerSettingsButtonsPanel, cancelRect, 120, "Textures/gameButtons.bmp");
	if(!cancelButton){
		return 1;
	}
	
	SDL_Rect setBoardRect = {8*TILE_SIZE, 0, 146, 40};
	Button* setBoardButton = Button_new(SET_BOARD, playerSettingsButtonsPanel, setBoardRect, 160, "Textures/gameButtons.bmp");
	if (!setBoardButton){
		return 1;
	}
	

	
	
	RadioGroup* minimaxDepthForHintRadioGroup = RadioGroup_new(&maxRecursionDepth);
	if(!minimaxDepthForHintRadioGroup){
		return 1;
	}
	
	for (int i = 0; i <= 4; i++){
		SDL_Rect crop = {0, i*24, 48, 24};
		SDL_Rect pos = {24+5.5*TILE_SIZE, i*24+2.5*TILE_SIZE, 48, 24};
		Radio* depthRadio = Radio_new("Textures/difficultyLabels.bmp", playerSettingsRadiosPanel, crop, pos, i);
		if (!depthRadio){
			return 1;
		}
		RadioGroup_add(minimaxDepthForHintRadioGroup, depthRadio);
		if (i == maxRecursionDepth){
			depthRadio->state = 1;
			depthRadio->group->selected = depthRadio;
		}
	}
	
	LinkedList_add(playerSettingsRadiosPanel->children, minimaxDepthForHintRadioGroup);	
	
	LinkedList_add(window->children, playerSettingsRadiosPanel);
	LinkedList_add(window->children, playerSettingsButtonsPanel);
	LinkedList_add(window->children, playerSettingsHeaderPanel);
	return 0;
}

int setScreenToSaveLoad(short save){
	prepareWindowForNewScreen();
	
	SDL_Rect rect = {0, 0, 768, 768};
	Panel* saveLoadPanel = save? 	Panel_new(window->surface, rect, &savePanel_draw):
									Panel_new(window->surface, rect, &loadPanel_draw);
	saveLoadPanel->children = LinkedList_new(&Button_free);
	LinkedList_add(window->children, saveLoadPanel);
	
	for (int i = 0; i < NUMBER_OF_SAVE_SLOTS; i++){
		if (!save){
			char buf[12];
			sprintf(buf, "slot%02d.xml", i);
			const char* path = &buf[0];
			if (access(path, R_OK)){
				continue;
			}
		}
		int row = i/4;
		int col = i%4;
		SDL_Rect pos = {128+col*128, 128+row*128, 128, 128};
		int buttonId = save? 100+i: 200+i;
		Button* button = Button_new(buttonId, saveLoadPanel, pos, 0, "Textures/saveSlots.bmp");
		if (!button){
			return 1;
		}
	}
	
	SDL_Rect pos = {311, 11*TILE_SIZE, 146, 40};
	int buttonId = save? RETURN_TO_GAME: MAIN_MENU;
	Button* button = Button_new(buttonId, saveLoadPanel, pos, 120, "Textures/gameButtons.bmp");
	if (!button){
		return 1;
	}
	return 0;
}

static void Window_free(){
	LinkedList_free(window->children);
	if (movesOfSelectedPiece){
		LinkedList_free(movesOfSelectedPiece);
	}
	
	LinkedList_removeAll(window->buttons);
	free(window->buttons);
	LinkedList_removeAll(window->radios);
	free(window->radios);
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
	copyOfMainBoard = NULL;
	SDL_WM_SetCaption("Chess", NULL);
	
	//create tree
	window = Window_new(768, 768);
	if(!window){
		return 1;
	}
	setScreenToMainMenu();
	return 0;
}

int GUI_paint(){
	// Clear window to background white
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
		if ((Rect_contains(button->absoluteRect, x, y)) && (button->hidden == 0)){
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
		if (Rect_contains(radio->absolutePos, x+24, y) && !(radio->hidden)){
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
