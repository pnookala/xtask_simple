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

static int QUEUE_TYPE = 1;

int cmpfunc(const void * a, const void * b) {
    return ( *(int*) a - *(int*) b);
}

void SortTicks(ticks* numTicks, int total) {
    qsort(numTicks, total, sizeof (*numTicks), cmpfunc);
}

int main(int argc, char **argv) {
    int *id = (int *) malloc(sizeof (int));
    //    void *params;

    *id = -1;

    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    pthread_setaffinity_np(pthread_self(), sizeof (set), &set);

    int threadCount = 0;
    int *threads = malloc(sizeof (int*));
    char* fileName1;

    if (argc != 4) {
        printf("Usage: <QueueType 1-SQueue, 2-CK, 3-Basic Queue, 4-Multiple Incoming Queues>, \nThreads-1,2,4,6,8,12,16,24,32,48,57,96,114,192,228,384,456,768,912,1024, \nRaw data file name: <name>,  \nSummary file name: <name>, \nClock Frequency in GHz: <3.4>\n");
        exit(-1);
    } else {
        char* arg = argv[1];
        QUEUE_TYPE = atoi(arg);

        switch (QUEUE_TYPE) {
            case 1:
                printf("Queue type: SQueue\n");
                break;
            case 2:
                printf("Queue type: CK\n");
                break;
            case 3:
                printf("Queue type: Basic Linux Queue\n");
                break;
            case 4:
                printf("Queue type: Multiple Incoming Queues (NumQueues=NumThreads/2) \n");
                break;
            case 5:
                printf("Queue type: RCU LF Queue\n");
                break;
            case 8:
                printf("Multiple Incoming Queues - Single producer multiple consumer variation\n");
                break;
            default:
                printf("Usage: <QueueType 1-SQueue, 2-CK, 3-Basic Queue, 4-Multiple Incoming Queues, 5-RCU LF Queue>, \nThreads-1,2,4,6,8,12,16,24,32,48,57,96,114,192,228,384,456,768,912,1024, \nRaw data file name: <name>,  \nSummary file name: <name>\n");
                exit(-1);
                break;
        }

        char* str = argv[2];
        char *thread;
        thread = strtok(str, ",");
        printf("Thread list: ");
        while (thread != NULL) {
            threads[threadCount] = atoi(thread);
            threadCount++;
            printf("%s ", thread);
            thread = strtok(NULL, ",");
        }

        printf("\n");

        fileName1 = argv[3];

        printf("Num of samples: %d\n", NUM_SAMPLES);
        printf("Thread list count: %d\n", threadCount);
        printf("Output files: %s\n", fileName1);
    }

#ifdef LATENCY  
    FILE *rfp = fopen(fileName1, "a");
    //fprintf(rfp, "QueueType NumSamples AverageDequeueCycles MinDequeueCycles MaxDequeueCycles NumThreads\n");
#endif
#ifdef THROUGHPUT
    FILE *rfp = fopen(fileName1, "a");
    //fprintf(rfp, "QueueType NumSamples Throughput NumThreads\n");
#endif 
#ifdef RAW
    fprintf(rfp, "QueueType NumSamples Cycles NumThreads\n");
#endif

    for (int k = 0; k < threadCount; k++) {
        int workers = 0;
        workers = threads[k];

        xtask_setup(NUM_SAMPLES, workers);

//        for (int i = 0; i < NUM_SAMPLES; i++) {
//            struct task_desc* task = (struct task_desc*) malloc(sizeof (struct task_desc));
//            task->task_type = 1;
//            task->task_id = i + 1;
//#ifdef mpmctest
//            //printf("enqueue ID %d\n", (i%workers));
//            xtask_push(task, (i % workers));
//#endif
//#ifdef spsctest
//            xtask_push(task);
//#endif
//        }
#ifdef mpmctest
        start_workers();
#endif

        //    for (int i = 0; i < NUM_SAMPLES; i++) {
        //        xtask_poll(id, &params);
        //        //printf("Result : %d \n", *id);
        //    }

        xtask_cleanup();

#ifdef LATENCY

#ifdef mpmctest
        SortTicks(mps->enqueuetimestamps, mps->totalenqueuesamples);
        SortTicks(mps->dequeuetimestamps, mps->totaldequeuesamples);
#endif
#ifdef spsctest
        SortTicks(mps->timestamps, NUM_SAMPLES);
#endif
#endif
        //printf("QueueType NumSamples AverageEnqueueCycles MinEnqueueCycles MaxEnqueueCycles AverageDequeueCycles MinDequeueCycles MaxDequeueCycles NumThreads\n");
#ifdef LATENCY
#ifdef spsctest
        ticks totalTicks = 0;
        for (int i = 0; i < mps->totalsamples; i++) {
            //printf("numsamples %ld i %d\n", mps->totalsamples, i);
            totalTicks += mps->timestamps[i];
        }

        ticks tickMin = mps->timestamps[0];
        ticks tickMax = mps->timestamps[mps->totalsamples - 1];

        ticks tickMedian = 0;

        if (mps->totalsamples % 2 == 0) {
            // if there is an even number of elements, return mean of the two elements in the middle
            tickMedian = ((mps->timestamps[(mps->totalsamples / 2)] + mps->timestamps[(mps->totalsamples / 2) - 1]) / 2.0);
        } else {
            // else return the element in the middle
            tickMedian = mps->timestamps[(mps->totalsamples / 2)];
        }
        //compute average
        double tickAverage = (totalTicks *1.0 / mps->totalsamples);

        fprintf(rfp, "%d %ld %lf %ld %ld %ld %d \n", QUEUE_TYPE, mps->totalsamples, tickAverage, tickMin, tickMax, tickMedian, workers);
        printf("%d %ld %lf %ld %ld %ld %d \n", QUEUE_TYPE, mps->totalsamples, tickAverage, tickMin, tickMax, tickMedian, workers);
#ifdef RAW
        int i = 0;
        while (i < NUM_SAMPLES) {
            fprintf(rfp, "%d %ld %ld %d\n", QUEUE_TYPE, mps->totalsamples, mps->timestamps[i], workers);
#ifdef VERBOSE
            printf("%d %ld %ld %d\n", QUEUE_TYPE, mps->totalsamples, mps->timestamps[i], workers);
#endif
            i++;
        }
#endif
#endif
#ifdef mpmctest
        ticks totalDequeueTicks = 0, totalEnqueueTicks = 0;
        for (int i = 0; i < mps->totaldequeuesamples; i++) {
            totalEnqueueTicks += mps->enqueuetimestamps[i];
            totalDequeueTicks += mps->dequeuetimestamps[i];
        }

        ticks tickEnqueueMin = mps->enqueuetimestamps[0];
        ticks tickEnqueueMax = mps->enqueuetimestamps[mps->totalenqueuesamples - 1];
        ticks tickDequeueMin = mps->dequeuetimestamps[0];
        ticks tickDequeueMax = mps->dequeuetimestamps[mps->totaldequeuesamples - 1];

        ticks tickMedian = 0;

        if (mps->totaldequeuesamples % 2 == 0) {
            // if there is an even number of elements, return mean of the two elements in the middle
            tickMedian = ((mps->dequeuetimestamps[(mps->totaldequeuesamples / 2)] + mps->dequeuetimestamps[(mps->totaldequeuesamples / 2) - 1]) / 2.0);
        } else {
            // else return the element in the middle
            tickMedian = mps->dequeuetimestamps[(mps->totaldequeuesamples / 2)];
        }

        //compute average
        //double tickEnqueueAverage = (totalEnqueueTicks / NUM_SAMPLES);
        double tickDequeueAverage = (totalDequeueTicks * 1.0 / mps->totaldequeuesamples);
        double tickEnqueueAverage = (totalEnqueueTicks * 1.0 / mps->totalenqueuesamples);

        fprintf(rfp, "%d %ld %lf %lf %ld %ld %ld %d \n", QUEUE_TYPE, mps->totaldequeuesamples, tickDequeueAverage, tickEnqueueAverage, tickDequeueMin, tickDequeueMax, tickMedian, workers);
        printf("%d %ld %lf %lf %ld %ld %ld %d \n", QUEUE_TYPE, mps->totaldequeuesamples, tickDequeueAverage, tickEnqueueAverage, tickDequeueMin, tickDequeueMax, tickMedian, workers);
#ifdef RAW
        int i = 0;
        while (i < NUM_SAMPLES) {
            fprintf(rfp, "%d %ld %ld %d\n", QUEUE_TYPE, mps->totaldequeuesamples, mps->dequeuetimestamps[i], workers);
#ifdef VERBOSE
            printf("%d %ld %ld %d\n", QUEUE_TYPE, mps->totaldequeuesamples, mps->dequeuetimestamps[i], workers);
#endif
            i++;
        }
#endif
#endif
#endif

#ifdef THROUGHPUT
#ifdef spsctest
        fprintf(rfp, "%d %ld %lf %d\n", QUEUE_TYPE, mps->totalsamples, mps->throughput, workers);
        printf("%d %ld %lf %d\n", QUEUE_TYPE, mps->totalsamples, mps->throughput, workers);
#endif
#ifdef mpmctest
        fprintf(rfp, "%d %ld %lf %d\n", QUEUE_TYPE, mps->totaldequeuesamples, mps->dequeuethroughput, workers);
        printf("%d %ld %lf %d\n", QUEUE_TYPE, mps->totaldequeuesamples, mps->dequeuethroughput, workers);
#endif
#endif
    }
    //while(1);
    free(id);

    return (0);
}


