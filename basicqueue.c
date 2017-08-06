#include "basicqueue.h"

void InitBasicQueue(int size)
{
	CIRCLEQ_INIT(&head);
	CIRCLEQ_INIT(&rq_head);
	pthread_mutex_init(&lock, NULL);
}

void BasicEnqueue(void* task)
{
	struct basicentry* n = (struct basicentry*) malloc(sizeof(struct basicentry));
	n->elem = task;
	pthread_mutex_lock(&lock);
	CIRCLEQ_INSERT_TAIL(&head, n, entries);
	pthread_mutex_unlock(&lock);
#ifdef VERBOSE
	printf("Added %d to the queue\n", ((struct task_desc*)(n->elem))->task_id);
#endif
}

void* BasicDequeue()
{
	// Remove a number from the queue
	struct basicentry *n;
	//Keep doing this until we dequeue an actual number, which also ensures queue is not empty for a successful dequeue.
	do
	{
		pthread_mutex_lock(&lock);
		n = CIRCLEQ_FIRST(&head);
		CIRCLEQ_REMOVE(&head, head.cqh_first, entries);
		pthread_mutex_unlock(&lock);
		sched_yield();
	}
	while(!n->elem);
#ifdef VERBOSE
	printf("Removed %d from the queue\n", ((struct task_desc*)(n->elem))->task_id);
#endif
	return n->elem;
}

void BasicEnqueue_rq(void* task)
{
	struct basicentry* n = (struct basicentry*) malloc(sizeof(struct basicentry));
	n->elem = task;
	pthread_mutex_lock(&lock);
	CIRCLEQ_INSERT_TAIL(&rq_head, n, entries);
	pthread_mutex_unlock(&lock);
#ifdef VERBOSE
	printf("Added %d to the result queue\n", ((struct task_desc*)(n->elem))->task_id);
#endif
}

void* BasicDequeue_rq()
{
	// Remove a number from the queue
	struct basicentry *n;
	do
	{
		pthread_mutex_lock(&lock);
		n = CIRCLEQ_FIRST(&rq_head);
		CIRCLEQ_REMOVE(&rq_head, rq_head.cqh_first, entries);
		pthread_mutex_unlock(&lock);
		sched_yield();
	}
	while(!n->elem);
#ifdef VERBOSE
	printf("Removed %d from the result queue\n", ((struct task_desc*)(n->elem))->task_id);
#endif
	return n->elem;
}
