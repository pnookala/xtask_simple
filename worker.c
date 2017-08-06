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
#include "worker.h"

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
    printf("Thread on CPU %d\n", sched_getcpu());
    
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
    double diff = 0.0;
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
#endif
#ifdef THROUGHPUT
    clock_gettime(CLOCK_MONOTONIC, &tend);
    pthread_mutex_lock(&lock);
    double elapsed = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1E9);
    printf("elapsed time: %lf\n", elapsed);
    mps->throughput += ((NUM_SAMPLES_PER_THREAD * 1.0) / elapsed);
    pthread_mutex_unlock(&lock);
#endif

    return NULL;
}


