/*
 * xtask_api.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#ifndef XTASK_API_H_
#define XTASK_API_H_

// This typedef can change, as long as semaphores are still equivalent-by-copy.
typedef unsigned long long xtask_sem_t;

// Structure for task-specific data. Notably does not contain the executable.
struct xtask_task_spec {
	int nwaits;
	xtask_sem_t *waits;
	int nsignals;
	xtask_sem_t *signals;
	void *data;
};

// Setup the XTask system. Should only be called once.
void xtask_setup(int queue_size, int workers);

// Wait for the given semaphores, kill the threads, and free all internal data.
void xtask_cleanup(int num, xtask_sem_t *sems);

// Initialize a number of semaphores.
void xtask_sem_init(int num, xtask_sem_t *sems);

// Destroy/free some semaphores. This should still need done even after _cleanup.
void xtask_sem_destroy(int num, xtask_sem_t *sems);

// Push a task onto the queue, which will not be executed before the semaphores
// in <spec.waits> have been signaled. It is expected that <task> will signal the
// semaphores specified in <spec.signals>, directly or indirectly.
void xtask_push(void (*task)(xtask_task_spec), xtask_task_spec spec);

// Signal some semaphores. Once signaled, a semaphore will always be signaled.
void xtask_signal(int num, xtask_sem_t *sems);

#endif /* XTASK_API_H_ */
