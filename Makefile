# Name: Alex Cooke (18406489)
# File name: Makefile
# Purpose: This Makefile is necessary for compiling
# and linking each file to create an executable scheduler.

CC = gcc
CFLAGS = -Wall -ansi -pedantic -Werror
OBJ = scheduler.o fileio.o
EXEC = scheduler

$(EXEC) : scheduler.o fileio.o
	$(CC) $(OBJ) -pthread -o $(EXEC)

scheduler.o : scheduler.c scheduler.h ready_queue.h fileio.h
	$(CC) -c scheduler.c $(CFLAGS)

fileio.o : fileio.c fileio.h
		$(CC) -c fileio.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
