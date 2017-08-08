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

#include "worker_mpmc.h"
#ifdef mpmctest
struct mproc_state *mps;

void xtask_setup(int queue_size, int workers);
void xtask_cleanup();

void xtask_push(struct task_desc* task, int queueID);
void xtask_poll(void* task);


#ifdef __cplusplus
}
#endif
#endif
#endif /* XTASK_MPMC_API_H */

