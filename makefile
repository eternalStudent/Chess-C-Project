CFLAGS = -std=c99 -g -Wall -pedantic-errors `sdl-config --cflags`
OFILES = Chess.o Board.o PossibleMove.o PossibleMoveList.o PieceCounter.o Iterator.o LinkedList.o GUI.o 

all: chessprog

clean:
	-rm chessprog $(OFILES)

chessprog: $(OFILES)
	gcc -o chessprog $(CFLAGS) $(OFILES) `sdl-config --libs` 
	
Chess.o: Chess.c Chess.h GUI.h
	gcc -c $(CFLAGS) Chess.c
	
Board.o: Board.c Board.h PossibleMove.h PossibleMoveList.h LinkedList.h
	gcc -c $(CFLAGS) Board.c

PossibleMove.o: PossibleMove.c PossibleMove.h Board.h
	gcc -c $(CFLAGS) PossibleMove.c

PossibleMoveList.o: PossibleMoveList.c PossibleMoveList.h PossibleMove.h Iterator.h LinkedList.h
	gcc -c $(CFLAGS) PossibleMoveList.c

PieceCounter.o: PieceCounter.c PieceCounter.h Board.h
	gcc -c $(CFLAGS) PieceCounter.c

Iterator.o: Iterator.c Iterator.h LinkedList.h
	gcc -c $(CFLAGS) Iterator.c
	
LinkedList.o: LinkedList.c LinkedList.h
	gcc -c $(CFLAGS) LinkedList.c
	
GUI.o: GUI.c Board.h LinkedList.h Iterator.h Chess.h PieceCounter.h
	gcc -c $(CFLAGS) -lm GUI.c 