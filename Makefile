CC=gcc
CFLAGS=-g

all: store_server

store_server: store_server.o store.o command_parser.o
	$(CC) command_parser.o store.o store_server.o -o store_server

store_server.o: store_server.c store.h
		$(CC) $(CFLAGS) -O3 -c -o store_server.o store_server.c

store.o: store.c store.h
	$(CC) $(CFLAGS) -O3 -c -o store.o store.c

command_parser.o: command_parser.c command_parser.h
	$(CC) $(CFLAGS) -O3 -c -o command_parser.o command_parser.c


clean:
	rm -rf *o *.out
