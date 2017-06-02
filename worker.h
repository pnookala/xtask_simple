/*
 * worker.h
 *
 *  Created on: Jun 2, 2017
 *      Author: pnookala
 */

#ifndef WORKER_H_
#define WORKER_H_

#include "basicqueue.h"

struct task_desc *execute_task(struct task_desc *task);
void *worker_handler(void *data);

#endif /* WORKER_H_ */
