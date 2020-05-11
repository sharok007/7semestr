CC=g++

CFLAGS=-c -Wall

PARALFLAGS=-fopenmp -lpthread

all: main

main: main.o vectorstruct.o
	$(CC) $(PARALFLAGS) main.o vectorstruct.o -o main

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

vectorstruct.o: vectorstruct.cpp
	$(CC) $(CFLAGS) vectorstruct.cpp

clean:
	rm -rf *.o main
