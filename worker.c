/*
 * worker.c
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#include "worker.h"

void *execute_task(struct task_desc *task)
{
	sleep(0);

	return;
}

void *worker_handler(void *data)
{
  struct mproc_state *mps = (struct mproc_state *) data;
	struct task_desc *task;

	while(!(*(mps->kill_master))) {
		task = dequeue(mps->incoming);
		task = execute_task(task);
		enqueue(task, mps->results);
	}

	return NULL;
}


/*
 * NOTES:
 * TODO: switch case numbers above should be in an enum
 */


