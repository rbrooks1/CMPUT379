CC=g++
CFLAGS=-c -Wall -std=c++11 -O -pthread
LFLAGS=
PROGS= prodcon
OBJS= prodcon.o

all: prodcon.o
	@$(CC) prodcon.o -o prodcon -pthread -O $(LFLAGS)

prodcon.o: prodcon.cpp
	@$(CC) prodcon.cpp -o prodcon.o $(CFLAGS)

clean:
	@rm $(PROGS)
	@rm $(OBJS)