CC=g++
CFLAGS=-c -Wall -std=c99 -std=c++11 -pthread
LFLAGS=
PROGS= server client
OBJS= server.o client.o

all: server client
	@$(CC) server.o -o server $(LFLAGS)
	@$(CC) client.o -o client $(LFLAGS)

client.o: client.cpp
	@$(CC) client.cpp -o client.o $(CFLAGS)

server.o: server.cpp
	@$(CC) server.cpp -o server.o $(CFLAGS)

tands.o: tands.c
	gcc tands.c -o tands.o $(CFLAGS)

clean:
	@rm $(PROGS)
	@rm $(OBJS)