/*
 * taskqueue.h
 *
 *  Created on: Jun 1, 2017
 *      Author: pnookala
 */

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <stdio.h>

typedef int atom;
static int queueSize;

// List Function Prototypes
void InitQueue(int size);             // Initialize the queue
void ClearQueue();            // Remove all items from the queue
void PrintQueue();            // Print all items from the queue
int Enqueue(atom elem);      // Enter an item in the queue
atom Dequeue();             // Remove an item from the queue
inline int isEmpty();                // Return true if queue is empty
inline int isFull();                 // Return true if queue is full

// Define TRUE and FALSE if they have not already been defined
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#endif /* TASKQUEUE_H_ */
