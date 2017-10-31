CC=gcc
CFLAGS=-Wall -g 

all: simple_main

simple_main: simple_main.o store.o
	$(CC) simple_main.o store.o -o simple_main

simple_main.o: simple_main.c
	$(CC) $(CFLAGS) -c simple_main.c

store.o: store.c store.h
	$(CC) $(CFLAGS) -O3 -c -o store.o store.c

clean:
	rm -rf *o *.out
