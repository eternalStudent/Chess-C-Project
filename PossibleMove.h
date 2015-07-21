struct PossibleMove{
	int fromX, fromY, toX, toY;
	char promotion;
	char** board;
};

struct PossibleMove* PossibleMove_new(int, int, int, int, char, char**);

int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other);

void PossibleMove_print(struct PossibleMove*);

struct PossibleMove* PossibleMove_clone (struct PossibleMove* move);

void PossibleMove_free(void*);