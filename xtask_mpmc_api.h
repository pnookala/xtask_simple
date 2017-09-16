/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   xtask_mpmc_api.h
 * Author: pnookala
 *
 * Created on August 6, 2017, 10:14 PM
 */

#ifndef XTASK_MPMC_API_H
#define XTASK_MPMC_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "squeuemultiple.h"
#ifdef mpmctest

#define NUM_SAMPLES 16//8388608 //2^23
//#define QUEUE_SIZE NUM_SAMPLES   // Define maximum length of the queue
#define NUM_CPUS 24

//#define INITQUEUES(size) InitBasicQueue(size)
//#define ENQUEUE(data) BasicEnqueue(data)
//#define DEQUEUE() BasicDequeue()
//#define ENQUEUE_RESULT(data) BasicEnqueue_rq(data)
//#define DEQUEUE_RESULT() BasicDequeue_rq()

#define INITQUEUES(size, numQueues) InitQueues(size, numQueues)
#define ENQUEUE(elem, queue, queueID) EnqueueMultiple(elem, queue, queueID)
#define DEQUEUE(q, queueID) DequeueMultiple(q, queueID)
//#define ENQUEUE_RESULT(data) BasicEnqueue_rq(data)
//#define DEQUEUE_RESULT() BasicDequeue_rq()

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
struct mproc_state *mps;

void xtask_setup(int queue_size, int workers);
void xtask_cleanup();

void xtask_push(struct task_desc* task, int queueID);
void start_workers();
void xtask_poll(void* task);


#ifdef __cplusplus
}
#endif
#endif
#endif /* XTASK_MPMC_API_H */

