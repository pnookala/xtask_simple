CC = gcc -g
#CC = icc -mmic	#use this for to make an executable for Phi
CFLAGS = -std=c99 -D_GNU_SOURCE -O2 -Wall -Wpedantic #-DVERBOSE
OBJECTS = basicqueue.o xtask_api.o squeuemultiple.o xtask_mpmc_api.o main.o
LDFLAGS = -lpthread

all:
	$(CC) $(CFLAGS) -c -DLATENCY -Dspsctest basicqueue.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dspsctest xtask_api.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dspsctest squeuemultiple.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dspsctest xtask_mpmc_api.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dspsctest main.c
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -DLATENCY -Dspsctest -o main
	
allmpmc:
	$(CC) $(CFLAGS) -c -DLATENCY -Dmpmctest basicqueue.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dmpmctest xtask_api.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dmpmctest squeuemultiple.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dmpmctest xtask_mpmc_api.c
	$(CC) $(CFLAGS) -c -DLATENCY -Dmpmctest main.c
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -DLATENCY -Dmpmctest -o main
	
th:
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dspsctest basicqueue.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dspsctest xtask_api.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dspsctest squeuemultiple.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dspsctest xtask_mpmc_api.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dspsctest main.c
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -DTHROUGHPUT -Dspsctest -o main
	
thmpmc:
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dmpmctest basicqueue.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dmpmctest xtask_api.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dmpmctest squeuemultiple.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dmpmctest xtask_mpmc_api.c
	$(CC) $(CFLAGS) -c -DTHROUGHPUT -Dmpmctest main.c
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -DTHROUGHPUT -Dmpmctest -o main

clean:
	rm -f *.o main
