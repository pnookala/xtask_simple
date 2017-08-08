/*
 * xtask_api.c
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include "xtask_api.h"
#include "basicqueue.h"
//#include "worker.h"
#ifdef spsctest

static int WORKERS = 0;
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

struct task_desc *execute_task(struct task_desc *task) {
    if(task)
    {
        switch(task->task_type)
        {
            case 0:
                sleep(0);
                break;
            case 1:
                __asm__ __volatile__("nop;");
                break;
            default:
                break;
        }
    }
    sleep(0);

    return 0;
}

void *worker_handler(void *data) {
    struct thread_local_data *d = (struct thread_local_data *) data;
    struct mproc_state *mps = d->mps;
    int cpuID = d->cpuID;
    struct task_desc *task;

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpuID, &set);

    pthread_setaffinity_np(pthread_self(), sizeof (set), &set);
    //printf("Thread on CPU %d\n", sched_getcpu());
    
    int NUM_SAMPLES_PER_THREAD = (NUM_SAMPLES / WORKERS);
#ifdef LATENCY
    ticks start_tick, end_tick;
    ticks *timestamp;
    timestamp = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES_PER_THREAD);
    for (int i = 0; i < NUM_SAMPLES_PER_THREAD; i++) {
        timestamp[i] = (ticks) 0;
    }
#endif
    pthread_barrier_wait(&mps->barrier);
#ifdef THROUGHPUT
//    double diff = 0.0;
    struct timespec tstart, tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
#endif
    //while(!(*(mps->kill_master))) {
    for (int count = 0; count < NUM_SAMPLES_PER_THREAD; count++) {
#ifdef LATENCY
        start_tick = getticks();
#endif
        task = DEQUEUE();
        execute_task(task);
        ENQUEUE_RESULT(task);
#ifdef LATENCY
        end_tick = getticks();
        timestamp[count] = end_tick - start_tick;
#endif
    }
#ifdef LATENCY
        pthread_mutex_lock(&lock);
        memcpy(mps->timestamps + mps->totalsamples, timestamp, NUM_SAMPLES_PER_THREAD * sizeof (ticks));
        mps->totalsamples += NUM_SAMPLES_PER_THREAD;
        pthread_mutex_unlock(&lock);
        free(timestamp);
#endif
#ifdef THROUGHPUT
    clock_gettime(CLOCK_MONOTONIC, &tend);
    pthread_mutex_lock(&lock);
    double elapsed = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1E9);
    //printf("elapsed time: %lf\n", elapsed);
    mps->throughput += ((NUM_SAMPLES_PER_THREAD * 1.0) / elapsed);
    pthread_mutex_unlock(&lock);
#endif

    return NULL;
}

#endif
#ifdef spsctest
void xtask_setup(int queue_size, int workers)
{
	int *kill_master = (int *) malloc(sizeof(int));
	*kill_master = 0;
        
        WORKERS = workers;

	mps = (struct mproc_state *) malloc(sizeof(struct mproc_state));
	//mps->incoming = CREATEQUEUE(queue_size);
	//mps->results = CREATEQUEUE(queue_size);
	INITQUEUES(queue_size);
	mps->kill_master = kill_master;
	mps->workers = workers;
	mps->worker_threads = (pthread_t *) malloc(sizeof(pthread_t) * workers);
        mps->timestamps = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES);
        
	pthread_barrier_init(&mps->barrier, NULL, workers);

	for (int t = 0; t < workers; t++)
	{
            struct thread_local_data* data = malloc(sizeof(struct thread_local_data*));
            data->mps = mps;
            data->cpuID = (t % NUM_CPUS);
		pthread_create(&mps->worker_threads[t], NULL, worker_handler, (void *)data);
	}

	//printf("created threads\n");
}

void xtask_cleanup()
{
	//TODO: We need to wait till input queue is empty before we kill the threads, however there is a timing issue here, processing might not be complete.
	//while(!CIRCLEQ_EMPTY(&head));
	//*(mps->kill_master) = 1;

	for (int t = 0; t < mps->workers; t++)
		pthread_join(mps->worker_threads[t], NULL);

	//dispose_queue(mps->incoming);
	//dispose_queue(mps->results);

	free(mps->worker_threads);
	free(mps->kill_master);
	free(mps);
}

void xtask_push(struct task_desc* task)
{
//        struct task_desc *task = (struct task_desc *) malloc(sizeof(struct task_desc));
//        task->task_id = id;
//        task->task_type = type;
//        task->num_threads = threads;
//        task->params = params;

	ENQUEUE(task);
}

void xtask_poll(void *task)
{

	task = DEQUEUE_RESULT();

	//assert(task != NULL);

	/* pass the pointers with the results */
	//*id = task->task_id;
	//*params = task->params;
	//*id = i;

	/*spsc cleanup task */
	//free(task);
}

#endif
