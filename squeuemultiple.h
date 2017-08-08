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
