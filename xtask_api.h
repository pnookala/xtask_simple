/*
 * xtask_api.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#ifndef XTASK_API_H_
#define XTASK_API_H_
#include "worker.h"

struct mproc_state *mps;

void xtask_setup(int queue_size, int workers);
void xtask_cleanup();

void xtask_push(struct task_desc* task);
void xtask_poll(void* task);

#endif /* XTASK_API_H_ */
