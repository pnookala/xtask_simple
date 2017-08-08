/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include "xtask_mpmc_api.h"
#include "squeuemultiple.h"
//#include "worker_mpmc.h"

#ifdef mpmctest
static int WORKERS = 0;
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
    //printf("Thread on CPU %d\n", sched_getcpu());
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
    //free(dequeuetimestamp);
#endif
#ifdef THROUGHPUT
    clock_gettime(CLOCK_MONOTONIC, &tend);
    pthread_mutex_lock(&lock);
    double elapsed = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1E9);
    //printf("elapsed time: %lf\n", elapsed);
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
    
    //free(enqueuetimestamp);
#endif
#ifdef THROUGHPUT
    clock_gettime(CLOCK_MONOTONIC, &tend);
    pthread_mutex_lock(&lock);
    double elapsed = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_nsec - tstart.tv_nsec) / 1E9);
    //printf("elapsed time: %lf\n", elapsed);
    mps->enqueuethroughput += ((NUM_SAMPLES_PER_THREAD * 1.0) / elapsed);
    pthread_mutex_unlock(&lock);
#endif

            return 0;
        }
#endif
    
#ifdef mpmctest
void xtask_setup(int queue_size, int workers) {
    int *kill_master = (int *) malloc(sizeof (int));
    *kill_master = 0;
    
    WORKERS = workers;

    int numProducers = 1;//workers / 2;
    int numConsumers = workers;

    INITQUEUES(queue_size, workers);

    mps = (struct mproc_state *) malloc(sizeof (struct mproc_state));
    mps->kill_master = kill_master;
    mps->workers = workers;
    mps->worker_threads = (pthread_t *) malloc(sizeof (pthread_t) * numConsumers);
    mps->enqueue_threads = (pthread_t *) malloc(sizeof (pthread_t) * numProducers);
    mps->enqueuetimestamps = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES);
    mps->dequeuetimestamps = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES);

    //printf("created threads\n");
}

void start_workers()
{
        pthread_barrier_init(&mps->barrier, NULL, WORKERS);

    for (int t = 0; t < WORKERS; t++) {
        struct thread_local_data* data = malloc(sizeof (struct thread_local_data*));
        data->mps = mps;
        data->cpuID = (t % WORKERS);
        pthread_create(&mps->enqueue_threads[t], NULL, enqueuemultiple_handler, (void *) data);
    }

    for (int t = 0; t < WORKERS; t++) {
        struct thread_local_data* data = malloc(sizeof (struct thread_local_data*));
        data->mps = mps;
        data->cpuID = (t % WORKERS);
        pthread_create(&mps->worker_threads[t], NULL, workermultiple_handler, (void *) data);
    }
}

void xtask_cleanup() {
    //TODO: We need to wait till input queue is empty before we kill the threads, however there is a timing issue here, processing might not be complete.
    //while(!CIRCLEQ_EMPTY(&head));
    //*(mps->kill_master) = 1;

    for (int i = 0; i < mps->workers; i++) {
        //pthread_join(mps->enqueue_threads[i], NULL);
        pthread_join(mps->worker_threads[i], NULL);
    }

    //dispose_queue(mps->incoming);
    //dispose_queue(mps->results);

//    free(mps->enqueue_threads);
//    free(mps->worker_threads);
//    free(mps->kill_master);
//    free(mps);
}

unsigned int rand_interval(unsigned int min, unsigned int max) {
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
}

void xtask_push(struct task_desc* task, int queueID) {
    //        struct task_desc *task = (struct task_desc *) malloc(sizeof(struct task_desc));
    //        task->task_id = id;
    //        task->task_type = type;
    //        task->num_threads = threads;
    //        task->params = params;
    //int pickedQ = rand_interval(0, WORKERS-1);
#ifdef VERBOSE
    printf("QueueID in xtask_push: %d\n", queueID);
#endif
    ENQUEUE(task, queues[queueID], queueID);
}

void xtask_poll(void *task) {

    task = DEQUEUE(resultQueue, 0);

    //assert(task != NULL);

    /* pass the pointers with the results */
    //*id = task->task_id;
    //*params = task->params;
    //*id = i;

    /* cleanup task */
    //free(task);
}

#endif
