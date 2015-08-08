CFLAGS = -std=c99 -g -Wall -pedantic-errors
OFILES = Board.o PossibleMove.o PossibleMoveList.o PieceCounter.o Iterator.o LinkedList.o

all: Chess

clean:
	-rm Chess $(OFILES)

Chess: Chess.c $(OFILES)
	gcc $(CFLAGS) -o $@ $^ 

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