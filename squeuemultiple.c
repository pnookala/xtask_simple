//---------------------------------------------------------------
// File: Code130_Queue.c
// Purpose: Implementation file for a demonstration of a queue
//		implemented as an array.    Data type: Character
// Programming Language: C
// Author: Dr. Rick Coleman
// Date: February 11, 2002
//---------------------------------------------------------------
#include <sys/types.h>
#include <stdlib.h>
#include <sched.h>
#include "squeuemultiple.h"

#ifdef mpmctest

int QUEUE_SIZE;

static inline void spinlock(volatile int *lock)
{
    while(!__sync_bool_compare_and_swap(lock, 0, 1))
    {
        sched_yield();
    }
}

static inline void spinunlock(volatile int *lock)
{
    *lock = 0;
}

//--------------------------------------------
// Function: InitQueue()
// Purpose: Initialize queue to empty.
// Returns: void
//--------------------------------------------
void InitQueues(int size, int numQueues)
{
	QUEUE_SIZE = (int)(size/numQueues);
#ifdef VERBOSE
	printf("Number of queues: %d\n", numQueues);
	printf("Queue Size: %d\n", QUEUE_SIZE);
#endif
	queues = (struct theQueue **) malloc(sizeof(struct theQueue*) * numQueues);
        resultQueue = (struct theQueue*)malloc(sizeof(struct theQueue));
        resultQueue->data = malloc(sizeof(struct task_desc*) * QUEUE_SIZE);
        
	for(int i=0; i<numQueues; i++)
	{
		struct theQueue *queue = malloc(sizeof(struct theQueue));
		queue->data = malloc(sizeof(struct task_desc*) * QUEUE_SIZE);
		queue->head = -1;
		queue->tail = -1;

		queues[i] = queue;
	}
}

//--------------------------------------------
// Function: ClearQueue()
// Purpose: Remove all items from the queue
// Returns: void
//--------------------------------------------
void ClearQueues()
{
    free(queues);
}

void PrintQueues()
{
//    printf("printing queue");
//    for (int i=0;i<MAX_SIZE;i++)
//    {
//        if (theQueue[i] != 0) printf("%d %d\n", i, theQueue[i]);
//    }
}


//--------------------------------------------
// Function: Enqueue()
// Purpose: Enqueue an item into the queue.
// Returns: TRUE if enqueue was successful
//		or FALSE if the enqueue failed.
// Note: We let head and tail continuing
//		increasing and use [head % MAX_SIZE]
//		and [tail % MAX_SIZE] to get the real
//		indices.  This automatically handles
//		wrap-around when the end of the array
//		is reached.
//--------------------------------------------
//should be thread safe, with no locks, just one atomic operation
inline int EnqueueMultiple(void* elem, struct theQueue *q, int queueID)
{
    // Check to see if the Queue is full
    //could pose problems in concurrent enqueue, perhaps leave extra space in the queue...
    //while ((tail - MAX_SIZE) == head);
    //check if its full, and wait...

    // Increment tail index
    int cur_tail = __sync_add_and_fetch(&q->tail,1);

    // Add the item to the Queue
    q->data[cur_tail % QUEUE_SIZE] = (struct task_desc*)(elem);
#ifdef VERBOSE
    printf("QueueID: %d, Enqueue: %d\n", queueID, ((struct task_desc*)(elem))->task_id);
#endif
    return TRUE;
}


//--------------------------------------------
// Function: Dequeue()
// Purpose: Dequeue an item from the Queue.
// Returns: TRUE if dequeue was successful
//		or FALSE if the dequeue failed.
//--------------------------------------------

inline void* DequeueMultiple(struct theQueue *q, int queueID)
{
	if(((q->tail - QUEUE_SIZE) > 0 && (q->head == q->tail)))
        {
#ifdef VERBOSE
            printf("Queue %d empty\n", queueID);
#endif
		return NULL;
        }
	struct task_desc* elem = malloc(sizeof(struct task_desc));
    int cur_head = __sync_add_and_fetch(&q->head,1);

    volatile struct task_desc *target = q->data[cur_head % QUEUE_SIZE];

    while (!target)
    {}

    *elem = *target;

#ifdef VERBOSE
    printf("QueueID: %d, Dequeue: %d\n", queueID, ((struct task_desc*)(elem))->task_id);
#endif
    return elem;
    
}

//--------------------------------------------
// Function: isEmpty()
// Purpose: Return true if the queue is empty
// Returns: TRUE if empty, otherwise FALSE
// Note: C has no boolean data type so we use
//	the defined int values for TRUE and FALSE
//	instead.
//--------------------------------------------
//inline int isEmpty()
//{
//    return (head == tail);
//}

//--------------------------------------------
// Function: isFull()
// Purpose: Return true if the queue is full.
// Returns: TRUE if full, otherwise FALSE
// Note: C has no boolean data type so we use
//	the defined int values for TRUE and FALSE
//	instead.
//--------------------------------------------
//inline int isFull()
//{
//    // Queue is full if tail has wrapped around
//    //	to location of the head.  See note in
//    //	Enqueue() function.
//    return ((tail - MAX_SIZE) >= head);
//}

#endif