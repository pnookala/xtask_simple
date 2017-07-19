/*
 * worker.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */
#ifndef WORKER_H_
#define WORKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "basicqueue.h"

#define NUM_SAMPLES 16//8388608 //2^23
#define QUEUE_SIZE NUM_SAMPLES   // Define maximum length of the queue
#define WORKERS 2
#define NUM_CPUS 2
#define QUEUE_TYPE 1

#define INITQUEUES(size) InitBasicQueue(size)
#define ENQUEUE(data) BasicEnqueue(data)
#define DEQUEUE() BasicDequeue()
#define ENQUEUE_RESULT(data) BasicEnqueue_rq(data)
#define DEQUEUE_RESULT() BasicDequeue_rq()

typedef long unsigned int ticks;

struct task_desc {
	int task_id;
	int task_type;
	int num_threads;
	void *params;
};

struct mproc_state {
	//struct queue *incoming;
	//struct queue *results;
	pthread_t *worker_threads;
	int *kill_master;
	int workers;
	pthread_barrier_t barrier;
	cpu_set_t cpuset;
        //For benchmarking purposes
        double throughput;
        ticks *timestamps;
        long int totalsamples;
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
void *worker_handler(void *data);

#endif /* WORKER_H_ */
