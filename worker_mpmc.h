/*
 * worker.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */
#ifndef WORKER_MPMC_H_
#define WORKER_MPMC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "squeuemultiple.h"
#ifdef mpmctest
#define NUM_SAMPLES 16//262144//8388608 //2^23
//#define QUEUE_SIZE NUM_SAMPLES   // Define maximum length of the queue
#define WORKERS 1
#define NUM_CPUS 2
#define QUEUE_TYPE 3
#define NUM_QUEUES WORKERS

//#define INITQUEUES(size) InitBasicQueue(size)
//#define ENQUEUE(data) BasicEnqueue(data)
//#define DEQUEUE() BasicDequeue()
//#define ENQUEUE_RESULT(data) BasicEnqueue_rq(data)
//#define DEQUEUE_RESULT() BasicDequeue_rq()

#define INITQUEUES(size, numQueues) InitQueues(size, numQueues)
#define ENQUEUE(elem, queue, queueID) EnqueueMultiple(elem, queue, queueID)
#define DEQUEUE(q, queueID) DequeueMultiple(q, queueID)
//#define ENQUEUE_RESULT(data) BasicEnqueue_rq(data)
//#define DEQUEUE_RESULT() BasicDequeue_rq()

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

#ifdef PHI

//get number of ticks, could be problematic on modern CPUs with out of order execution

static __inline__ ticks getticks(void) {
    ticks tsc;
    __asm__ __volatile__(
            "rdtsc;"
            "shl $32, %%rdx;"
            "or %%rdx, %%rax"
            : "=a"(tsc)
            :
            : "%rcx", "%rdx");

    return tsc;
}
#else
static __inline__ ticks getticks(void) {
    ticks tsc;
    __asm__ __volatile__(
            "rdtscp;"
            "shl $32, %%rdx;"
            "or %%rdx, %%rax"
            : "=a"(tsc)
            :
            : "%rcx", "%rdx");

    return tsc;
}
#endif

//static pthread_barrier_t barrier;

struct task_desc *execute_task(struct task_desc *task);
void *enqueuemultiple_handler(void * data);
void *workermultiple_handler(void * data);
#endif
#endif /* WORKER_MPMC_H_ */
