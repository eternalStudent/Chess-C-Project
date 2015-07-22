typedef struct Board{
	char** matrix;
	int kingX[2];
	int kingY[2];
} Board;

struct PossibleMove{
	int fromX, fromY, toX, toY;
	char promotion;
	Board* board;
};

struct PossibleMove* PossibleMove_new(int, int, int, int, char, Board*);

int PossibleMove_equals(struct PossibleMove* this, struct PossibleMove* other);

void PossibleMove_print(struct PossibleMove*);

struct PossibleMove* PossibleMove_clone (struct PossibleMove* move);

void PossibleMove_free(void*);