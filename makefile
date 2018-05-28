BIN=sudoku
CC=gcc
LB=-L library
LD=-lpthread -lm
FLAGS=-O3

all: sudoku.c
	$(CC) -o $(BIN) sudoku.c library/stack.c $(LB) $(LD) $(FLAGS)

clean:
	rm -rf $(BIN) *~