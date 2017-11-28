CC=gcc
CFLAGS=-g

all: simple_main

simple_main: simple_main.o store.o command_parser.o server.o
	$(CC) command_parser.o simple_main.o store.o server.o -o simple_main

simple_main.o: simple_main.c
	$(CC) $(CFLAGS) -c simple_main.c

server.o: server.c server.h store.h
		$(CC) $(CFLAGS) -O3 -c -o server.o server.c

store.o: store.c store.h
	$(CC) $(CFLAGS) -O3 -c -o store.o store.c

command_parser.o: command_parser.c command_parser.h
	$(CC) $(CFLAGS) -O3 -c -o command_parser.o command_parser.c


clean:
	rm -rf *o *.out
