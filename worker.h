/*
 * worker.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include "basicqueue.h"

#ifndef WORKER_H_
#define WORKER_H_

#define NUM_SAMPLES 12//8388608 //2^23
#define QUEUE_SIZE NUM_SAMPLES   // Define maximum length of the queue
#define WORKERS  2
#define NUM_CPUS 2

#define INITQUEUES(size) InitBasicQueue(size)
#define ENQUEUE(data) BasicEnqueue(data)
#define DEQUEUE() BasicDequeue()
#define ENQUEUE_RESULT(data) BasicEnqueue_rq(data)
#define DEQUEUE_RESULT() BasicDequeue_rq()

struct mproc_state {
	//struct queue *incoming;
	//struct queue *results;
	pthread_t *worker_threads;
	int *kill_master;
	int workers;
};

//static pthread_barrier_t barrier;

struct task_desc *execute_task(struct task_desc *task);
void *worker_handler(void *data);

#endif /* WORKER_H_ */
