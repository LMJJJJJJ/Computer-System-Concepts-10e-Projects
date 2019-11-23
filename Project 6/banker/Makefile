# makefile for Banker's Algorithm

CC=gcc
CFLAGS=-Wall

all: main.o banker.o
	$(CC) $(CFLAGS) -o main main.o banker.o

buffer.o: banker.c banker.h
	$(CC) $(CFLAGS) -c banker.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -rf *.o
	rm -rf main