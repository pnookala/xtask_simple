/*
 * main.c
 *
 *  Created on: May 31, 2017
 *      Author: pnookala
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sched.h>
#include <time.h>
#ifdef spsctest
#include "worker.h"
#include "basicqueue.h"
#include "xtask_api.h"
#endif
#ifdef mpmctest
#include "worker_mpmc.h"
#include "squeuemultiple.h"
#include "xtask_mpmc_api.h"
#endif

int cmpfunc(const void * a, const void * b) {
    return ( *(int*) a - *(int*) b);
}

void SortTicks(ticks* numTicks, int total) {
    qsort(numTicks, total, sizeof (*numTicks), cmpfunc);
}

int main(void) {
    int *id = (int *) malloc(sizeof (int));
//    void *params;

    *id = -1;

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    pthread_setaffinity_np(pthread_self(), sizeof (set), &set);
                                    
    xtask_setup(NUM_SAMPLES, WORKERS);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        struct task_desc* task = (struct task_desc*)malloc(sizeof(struct task_desc));
        task->task_type = 1;
        task->task_id = i+1;
#ifdef mpmctest
        xtask_push(task, (i%(NUM_QUEUES)));
#endif
#ifdef spsctest
        xtask_push(task);
#endif
    }

//    for (int i = 0; i < NUM_SAMPLES; i++) {
//        xtask_poll(id, &params);
//        //printf("Result : %d \n", *id);
//    }
    
    xtask_cleanup();

#ifdef LATENCY
    FILE *rfp = fopen("LatencyResults", "a");
#ifdef mpmctest
    SortTicks(mps->enqueuetimestamps, NUM_SAMPLES);
    SortTicks(mps->dequeuetimestamps, NUM_SAMPLES);
#endif
#ifdef spsctest
    SortTicks(mps->timestamps, NUM_SAMPLES);
#endif
#endif
#ifdef THROUGHPUT
    FILE *rfp = fopen("ThroughputResults", "a");
#endif 
#ifdef LATENCY
    fprintf(rfp, "QueueType NumSamples AverageEnqueueCycles MinEnqueueCycles MaxEnqueueCycles AverageDequeueCycles MinDequeueCycles MaxDequeueCycles NumThreads\n");
#ifdef RAW
    fprintf(rfp, "QueueType NumSamples Cycles NumThreads\n");
#endif
#endif
    //printf("QueueType NumSamples AverageEnqueueCycles MinEnqueueCycles MaxEnqueueCycles AverageDequeueCycles MinDequeueCycles MaxDequeueCycles NumThreads\n");
#ifdef LATENCY
#ifdef spsctest
    ticks totalTicks = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        totalTicks += mps->timestamps[i];
    }

    ticks tickMin = mps->timestamps[0];
    ticks tickMax = mps->timestamps[NUM_SAMPLES - 1];

    //compute average
    double tickAverage = (totalTicks / NUM_SAMPLES);
    
    fprintf(rfp, "%d %d %lf %ld %ld %d \n", QUEUE_TYPE, NUM_SAMPLES, tickAverage, tickMin, tickMax, WORKERS);
    printf("%d %d %lf %ld %ld %d \n", QUEUE_TYPE, NUM_SAMPLES, tickAverage, tickMin, tickMax, WORKERS);
#ifdef RAW
    int i = 0;
    while (i < NUM_SAMPLES) {
        fprintf(rfp, "%d %d %ld %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->timestamps[i], WORKERS);
#ifdef VERBOSE
        printf("%d %d %ld %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->timestamps[i], WORKERS);
#endif
        i++;
    }
#endif
#endif
#ifdef mpmctest
        ticks totalEnqueueTicks = 0, totalDequeueTicks = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        totalEnqueueTicks += mps->enqueuetimestamps[i];
        totalDequeueTicks += mps->dequeuetimestamps[i];
    }

    ticks tickEnqueueMin = mps->enqueuetimestamps[0];
    ticks tickEnqueueMax = mps->enqueuetimestamps[NUM_SAMPLES - 1];
    ticks tickDequeueMin = mps->dequeuetimestamps[0];
    ticks tickDequeueMax = mps->dequeuetimestamps[NUM_SAMPLES - 1];

    //compute average
    double tickEnqueueAverage = (totalEnqueueTicks / NUM_SAMPLES);
    double tickDequeueAverage = (totalDequeueTicks / NUM_SAMPLES);
    
    fprintf(rfp, "%d %d %lf %ld %ld %lf %ld %ld %d \n", QUEUE_TYPE, NUM_SAMPLES, tickEnqueueAverage, tickEnqueueMin, tickEnqueueMax, tickDequeueAverage, tickDequeueMin, tickDequeueMax, WORKERS);
    printf("%d %d %lf %ld %ld %lf %ld %ld %d \n", QUEUE_TYPE, NUM_SAMPLES, tickEnqueueAverage, tickEnqueueMin, tickEnqueueMax, tickDequeueAverage, tickDequeueMin, tickDequeueMax, WORKERS);
#ifdef RAW
    int i = 0;
    while (i < NUM_SAMPLES) {
        fprintf(rfp, "%d %d %ld %ld %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->enqueuetimestamps[i], mps->dequeuetimestamps[i], WORKERS);
#ifdef VERBOSE
        printf("%d %d %ld %ld %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->enqueuetimestamps[i], mps->dequeuetimestamps[i], WORKERS);
#endif
        i++;
    }
#endif
#endif
#endif

#ifdef THROUGHPUT
#ifdef spsctest
    fprintf(rfp, "QueueType NumSamples Throughput NumThreads\n");
    fprintf(rfp, "%d %d %lf %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->throughput, WORKERS);
    printf("%d %d %lf %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->throughput, WORKERS);
#endif
#ifdef mpmctest
    fprintf(rfp, "QueueType NumSamples EnqueueThroughput DequeueThroughput NumThreads\n");
    fprintf(rfp, "%d %d %lf %lf %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->enqueuethroughput, mps->dequeuethroughput, WORKERS);
    printf("%d %d %lf %lf %d\n", QUEUE_TYPE, NUM_SAMPLES, mps->enqueuethroughput, mps->dequeuethroughput, WORKERS);
#endif
#endif

    //while(1);
    free(id);

    return (0);
}


