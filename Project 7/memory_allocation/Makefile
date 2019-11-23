# makefile for Contiguous Memory Allocation

CC=gcc
CFLAGS=-Wall

all: allocator.o memory.o
	$(CC) $(CFLAGS) -o allocator allocator.o memory.o

buffer.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

allocator.o: allocator.c
	$(CC) $(CFLAGS) -c allocator.c

clean:
	rm -rf *.o
	rm -rf allocator