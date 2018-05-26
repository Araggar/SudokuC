BIN=sudoku
CC=gcc
LB=-L ./library/
LD=-lstack -lpthread 
FLAGS=-O3

all: sudoku.c
	$(CC) -o $(BIN) sudoku.c $(LB) $(LD) $(FLAGS)

clean:
	rm -rf $(BIN) *~