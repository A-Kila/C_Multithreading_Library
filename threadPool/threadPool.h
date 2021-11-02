#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdbool.h>
#include <pthread.h>

#include "../myQueue/myQueue.h"

typedef void *(*Func)(void *);

typedef struct {
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
    pthread_t *threads;
    int poolSize;
    Queue *queue;
    bool stopWork;
} tpool_t;


void tpoolInit(tpool_t *tpool, int poolSize);

/*
 * Funtion: tpoolSchedule
 * ---------------------------
 * Adds a function to a queue of functions.
 * The threads execute these funtions in order.
 * For example: 
 *  if we have 2 free threads and the queue size is 4,
 *  queue pops 2 functions and assigns them to the free threads.
 */
void tpoolSchedule(tpool_t *tpool, Func func, void *args);


void tpoolDestroy(tpool_t *tpool);


#endif
