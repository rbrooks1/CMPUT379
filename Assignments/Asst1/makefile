CC=g++
CFLAGS=-c -Wall -std=c++11
LFLAGS=
PROGS= shell
OBJS= Shell379.o

all: shell.o
	@$(CC) Shell379.o shell $(LFLAGS)

shell.o: Shell379.cpp
	@$(CC) Shell379.cpp -o Shell379.o $(CFLAGS)

clean:
	@rm $(PROGS)
	@rm $(OBJS)