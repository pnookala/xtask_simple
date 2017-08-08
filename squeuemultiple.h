//---------------------------------------------------------------
// File: Code130_Queue.h
// Purpose: Header file for a demonstration of a queue implemented
//        as an array.  Data type: Character
// Programming Language: C
// Author: Dr. Rick Coleman
//---------------------------------------------------------------
#ifndef SQUEUEMULTIPLE_H
#define SQUEUEMULTIPLE_H

#include <stdio.h>
#include "worker_mpmc.h"
#ifdef mpmctest

typedef long unsigned int ticks;

struct task_desc {
	int task_id;
	int task_type;
	int num_threads;
	void *params;
};

struct mproc_state {
	pthread_t *worker_threads;
        pthread_t *enqueue_threads;
	int *kill_master;
	int workers;
	pthread_barrier_t barrier;
	cpu_set_t cpuset;
        //For benchmarking purposes
        double dequeuethroughput;
        double enqueuethroughput;
        ticks *dequeuetimestamps;
        ticks *enqueuetimestamps;
        long int totalenqueuesamples;
        long int totaldequeuesamples;
};

struct thread_local_data
{
    struct mproc_state* mps;
    int cpuID;
};

struct theQueue
{
	int head;
	int tail;
	void **data __attribute__((aligned (4096)));
};

struct theQueue **queues;
struct theQueue *resultQueue;

// List Function Prototypes
void InitQueues(int size, int numQueues);             // Initialize the queue
void ClearQueues();            // Remove all items from the queue
void PrintQueues();            // Print all items from the queue
int EnqueueMultiple(void* elem, struct theQueue *queue, int queueID);      // Enter an item in the queue
void* DequeueMultiple(struct theQueue *q, int queueID);             // Remove an item from the queue
//inline int isEmpty();                // Return true if queue is empty
//inline int isFull();                 // Return true if queue is full

// Define TRUE and FALSE if they have not already been defined
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif
#endif
#endif // End of queue header
