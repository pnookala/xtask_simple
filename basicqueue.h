/*
 * basicqueue.h
 *
 *  Created on: Feb 20, 2017
 *      Author: pnookala
 */

#ifndef BASICQUEUE_H_
#define BASICQUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <pthread.h>

CIRCLEQ_HEAD(circleq, basicentry) head;
struct circleq *headp;              /* Circular queue head. */
struct basicentry {
  CIRCLEQ_ENTRY(basicentry) entries;   /* Circular queue. */
  int elem;
};

CIRCLEQ_HEAD(circleq_r, basicentry) rq_head;
struct circleq_r *rq_headp;              /* Circular queue head. */

pthread_mutex_t lock;

void InitBasicQueue(int size);
void BasicEnqueue(int i);
int BasicDequeue();

void BasicEnqueue_rq(int i);
int BasicDequeue_rq();

#endif /* BASICQUEUE_H_ */
