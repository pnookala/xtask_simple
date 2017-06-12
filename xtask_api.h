/*
 * xtask_api.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#ifndef XTASK_API_H_
#define XTASK_API_H_

// After-n primitive, only triggers on the nth usage.
typedef struct _xtask_aftern_internal* xtask_aftern_t;

// Task structure, to aid in pushing and such.
// <task> is called with a true <is_nth> if it is the nth task with the same
// <aftern> to be executed.
typedef struct {
	void (*task)(void* state, void* data, int is_nth);
	void* data;
	xtask_aftern_t aftern;
} xtask_task_t;

// Setup the XTask system. Should only be called once.
void xtask_setup(void* (*init_state)(), int queue_size, int workers);

// Cleanup the system, after all the tasks have completed.
void xtask_cleanup();

// Create a new after-n primitive with the given n. Should be used n times,
// since it is destroyed after the nth usage.
xtask_aftern_t xtask_aftern_create(int n);

// Push a new copy of a task into the system, which will be executed async.
void xtask_push(const xtask_task_t* task);

#endif /* XTASK_API_H_ */
