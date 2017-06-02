/*
 * main.c
 *
 *  Created on: May 31, 2017
 *      Author: pnookala
 */

#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sched.h>
#include <time.h>
#include "worker.h"
#include "basicqueue.h"
#include "squeue.h"

#define QUEUE_SIZE 8,388,608 //2^23   // Define maximum length of the queue


