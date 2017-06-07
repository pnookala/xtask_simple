CC = gcc -g
#CC = icc -mmic	#use this for to make an executable for Phi
CFLAGS = -std=c99 -D_GNU_SOURCE -O2 -Wall -Wpedantic -Werror #-DVERBOSE
OBJECTS = basicqueue.o xtask_api.o worker.o main.o
LDFLAGS = -lpthread

all:
	$(CC) $(CFLAGS) -c basicqueue.c
	$(CC) $(CFLAGS) -c xtask_api.c
	$(CC) $(CFLAGS) -c worker.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o main

clean:
	rm -f *.o main
