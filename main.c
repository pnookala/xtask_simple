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
#include "worker.h"
#include "basicqueue.h"
#include "xtask_api.h"

int cmpfunc(const void * a, const void * b) {
    return ( *(int*) a - *(int*) b);
}

void SortTicks(ticks* numTicks, int total) {
    qsort(numTicks, total, sizeof (*numTicks), cmpfunc);
}

int main(void) {
    int *id = (int *) malloc(sizeof (int));
    void *params;

    *id = -1;

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    pthread_setaffinity_np(pthread_self(), sizeof (set), &set);
                                    
    xtask_setup(QUEUE_SIZE, WORKERS);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        xtask_push(1, 1, i + 1, NULL);
    }

    for (int i = 0; i < NUM_SAMPLES; i++) {
        xtask_poll(id, &params);
        //printf("Result : %d \n", *id);
    }
    
    xtask_cleanup();

#ifdef LATENCY
    FILE *rfp = fopen("LatencyResults", "a");
    SortTicks(mps->timestamps, NUM_SAMPLES);
#endif
#ifdef THROUGHPUT
    FILE *rfp = fopen("ThroughputResults", "a");
#endif 
#ifdef LATENCY
    fprintf(rfp, "QueueType NumSamples AverageCycles MinCycles MaxCycles NumThreads\n");
#ifdef RAW
    fprintf(rfp, "QueueType NumSamples Cycles NumThreads\n");
#endif
#endif
    //printf("QueueType NumSamples AverageCycles MinCycles MaxCycles NumThreads\n");
#ifdef LATENCY
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

#ifdef THROUGHPUT
    fprintf(rfp, "QueueType NumSamples Throughput NumThreads\n");
    fprintf(rfp, "%d %d %lf %d\n", type, NUM_SAMPLES, mps->throughput, WORKERS);
    printf("%d %d %lf %d\n", type, NUM_SAMPLES, mps->throughput, WORKERS);
#endif

    //while(1);
    free(id);

    return (0);
}


