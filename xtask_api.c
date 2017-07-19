/*
 * xtask_api.c
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

#include "xtask_api.h"
#include "basicqueue.h"
#include "worker.h"

void xtask_setup(int queue_size, int workers)
{
	int *kill_master = (int *) malloc(sizeof(int));
	*kill_master = 0;

	mps = (struct mproc_state *) malloc(sizeof(struct mproc_state));
	//mps->incoming = CREATEQUEUE(queue_size);
	//mps->results = CREATEQUEUE(queue_size);
	INITQUEUES(queue_size);
	mps->kill_master = kill_master;
	mps->workers = workers;
	mps->worker_threads = (pthread_t *) malloc(sizeof(pthread_t) * workers);
        mps->timestamps = (ticks *) malloc(sizeof (ticks) * NUM_SAMPLES);
        
	pthread_barrier_init(&mps->barrier, NULL, workers);

	for (int t = 0; t < workers; t++)
	{
            struct thread_local_data* data = malloc(sizeof(struct thread_local_data*));
            data->mps = mps;
            data->cpuID = (t % NUM_CPUS);
		pthread_create(&mps->worker_threads[t], NULL, worker_handler, (void *)data);
	}

	//printf("created threads\n");
}

void xtask_cleanup()
{
	//TODO: We need to wait till input queue is empty before we kill the threads, however there is a timing issue here, processing might not be complete.
	//while(!CIRCLEQ_EMPTY(&head));
	//*(mps->kill_master) = 1;

	for (int t = 0; t < mps->workers; t++)
		pthread_join(mps->worker_threads[t], NULL);

	//dispose_queue(mps->incoming);
	//dispose_queue(mps->results);

	free(mps->worker_threads);
	free(mps->kill_master);
	free(mps);
}

void xtask_push(int type, int threads, int id, void *params)
{
	//	struct task_desc *task = (struct task_desc *) malloc(sizeof(struct task_desc));
	//	task->task_id = id;
	//	task->task_type = type;
	//	task->num_threads = threads;
	//	task->params = params;

	ENQUEUE(id);
}

void xtask_poll(int *id, void **params)
{
	//struct task_desc *task;

	int i = DEQUEUE_RESULT();

	//assert(task != NULL);

	/* pass the pointers with the results */
	//*id = task->task_id;
	//*params = task->params;
	*id = i;

	/* cleanup task */
	//free(task);
}
