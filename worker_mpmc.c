/*
 * worker.c
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include "worker_mpmc.h"
#ifdef mpmctest
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct task_desc *execute_task(struct task_desc *task) {
    if (task) {
        switch (task->task_type) {
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

void *workermultiple_handler(void * data) {
    struct thread_local_data *d = (struct thread_local_data *) data;
    struct mproc_state *mps = d->mps;
    int cpuID = d->cpuID;
    struct task_desc *task;

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpuID, &set);

    pthread_setaffinity_np(pthread_self(), sizeof (set), &set);
    printf("Thread on CPU %d\n", sched_getcpu());
    int NUM_SAMPLES_PER_THREAD = (NUM_SAMPLES / WORKERS);
    
#ifdef LATENCY
    ticks start_tick, end_tick;
    
    ticks *dequeuetimestamp;
    dequeuetimestamp = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES_PER_THREAD);
    for (int i = 0; i < NUM_SAMPLES_PER_THREAD; i++) {
        dequeuetimestamp[i] = (ticks) 0;
    }
#endif
    pthread_barrier_wait(&mps->barrier);
#ifdef THROUGHPUT
    //double diff = 0.0;
    struct timespec tstart, tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
#endif
    //while(!(*(mps->kill_master))) {
    for (int count = 0; count < NUM_SAMPLES_PER_THREAD; count++) {
#ifdef LATENCY
        start_tick = getticks();
#endif
        task = DEQUEUE(queues[cpuID], cpuID);
        execute_task(task);
        ENQUEUE(task, resultQueue, 0);
#ifdef LATENCY
        end_tick = getticks();
        dequeuetimestamp[count] = end_tick - start_tick;
#endif
    }
#ifdef LATENCY
    pthread_mutex_lock(&lock);
    memcpy(mps->dequeuetimestamps + mps->totaldequeuesamples, dequeuetimestamp, NUM_SAMPLES_PER_THREAD * sizeof (ticks));
    mps->totaldequeuesamples += NUM_SAMPLES_PER_THREAD;
    pthread_mutex_unlock(&lock);
#endif
#ifdef THROUGHPUT
    clock_gettime(CLOCK_MONOTONIC, &tend);
    pthread_mutex_lock(&lock);
    double elapsed = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1E9);
    printf("elapsed time: %lf\n", elapsed);
    mps->dequeuethroughput += ((NUM_SAMPLES_PER_THREAD * 1.0) / elapsed);
    pthread_mutex_unlock(&lock);
#endif

        return 0;

    }

    void *enqueuemultiple_handler(void * data) {
    struct thread_local_data *d = (struct thread_local_data *) data;
    struct mproc_state *mps = d->mps;
    int cpuID = d->cpuID;

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpuID, &set);

    pthread_setaffinity_np(pthread_self(), sizeof (set), &set);
    printf("Thread on CPU %d\n", sched_getcpu());
    
    int NUM_SAMPLES_PER_THREAD = (NUM_SAMPLES / WORKERS);
    
#ifdef LATENCY
    ticks start_tick, end_tick;
    
    ticks *enqueuetimestamp;
    enqueuetimestamp = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES_PER_THREAD);
    for (int i = 0; i < NUM_SAMPLES_PER_THREAD; i++) {
        enqueuetimestamp[i] = (ticks) 0;
    }
#endif
    pthread_barrier_wait(&mps->barrier);
#ifdef THROUGHPUT
    //double diff = 0.0;
    struct timespec tstart, tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);
#endif
    //while(!(*(mps->kill_master))) {
    for (int count = 0; count < NUM_SAMPLES_PER_THREAD; count++) {

        struct task_desc* task = (struct task_desc*)malloc(sizeof(struct task_desc));
        task->task_type = 1;
        task->task_id = count+1;
 #ifdef LATENCY       
        start_tick = getticks();
#endif
        ENQUEUE(task, queues[cpuID], cpuID);
         //EnqueueMultiple(task, queues[count % WORKERS], (count % WORKERS));
#ifdef LATENCY
        end_tick = getticks();
        enqueuetimestamp[count] = end_tick - start_tick;
#endif
    }
#ifdef LATENCY
    pthread_mutex_lock(&lock);
    memcpy(mps->enqueuetimestamps + mps->totalenqueuesamples, enqueuetimestamp, NUM_SAMPLES_PER_THREAD * sizeof (ticks));
    mps->totalenqueuesamples += NUM_SAMPLES_PER_THREAD;
    pthread_mutex_unlock(&lock);
#endif
#ifdef THROUGHPUT
    clock_gettime(CLOCK_MONOTONIC, &tend);
    pthread_mutex_lock(&lock);
    double elapsed = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1E9);
    printf("elapsed time: %lf\n", elapsed);
    mps->enqueuethroughput += ((NUM_SAMPLES_PER_THREAD * 1.0) / elapsed);
    pthread_mutex_unlock(&lock);
#endif

            return 0;
        }
#endif

