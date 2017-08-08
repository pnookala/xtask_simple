/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

#include "xtask_mpmc_api.h"
#include "squeuemultiple.h"
#include "worker_mpmc.h"
#ifdef mpmctest
void xtask_setup(int queue_size, int workers) {
    int *kill_master = (int *) malloc(sizeof (int));
    *kill_master = 0;

    int numProducers = 1;//workers / 2;
    int numConsumers = workers;

    INITQUEUES(queue_size, WORKERS);

    mps = (struct mproc_state *) malloc(sizeof (struct mproc_state));
    mps->kill_master = kill_master;
    mps->workers = workers;
    mps->worker_threads = (pthread_t *) malloc(sizeof (pthread_t) * numConsumers);
    mps->enqueue_threads = (pthread_t *) malloc(sizeof (pthread_t) * numProducers);
    mps->enqueuetimestamps = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES);
    mps->dequeuetimestamps = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES);

    pthread_barrier_init(&mps->barrier, NULL, workers);

//    for (int t = 0; t < numProducers; t++) {
//        struct thread_local_data* data = malloc(sizeof (struct thread_local_data*));
//        data->mps = mps;
//        data->cpuID = (t % NUM_QUEUES);
//        pthread_create(&mps->enqueue_threads[t], NULL, enqueuemultiple_handler, (void *) data);
//    }

    for (int t = 0; t < numConsumers; t++) {
        struct thread_local_data* data = malloc(sizeof (struct thread_local_data*));
        data->mps = mps;
        data->cpuID = (t % NUM_QUEUES);
        pthread_create(&mps->worker_threads[t], NULL, workermultiple_handler, (void *) data);
    }

    //printf("created threads\n");
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

    free(mps->worker_threads);
    free(mps->enqueue_threads);
    free(mps->kill_master);
    free(mps);
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
