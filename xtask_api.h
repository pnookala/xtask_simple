/*
 * xtask_api.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#ifndef XTASK_API_H_
#define XTASK_API_H_
#include "worker.h"

#ifdef spsctest

#define NUM_SAMPLES 16//262144//8388608 //2^23
#define QUEUE_SIZE NUM_SAMPLES   // Define maximum length of the queue
#define NUM_CPUS 2
static int WORKERS = 0;
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
struct mproc_state *mps;

void xtask_setup(int queue_size, int workers);
void xtask_cleanup();

void xtask_push(struct task_desc* task);
void xtask_poll(void* task);
#endif
#endif /* XTASK_API_H_ */
