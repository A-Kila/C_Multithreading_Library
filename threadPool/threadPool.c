#include <stdlib.h>
#include <assert.h>

#include "threadPool.h"

typedef struct {
    void *args;
    Func func;
} tpool_args;

/*
 * Function: threadWork
 * --------------------
 * A helper function for tpoolInit.
 * Checks if there are any more tasks for the threads 
 * and if yes, assignes them to free threads, or waits for 
 * one of them to stop working.
 * @params args - tpool_t pointer cast to a void *
 */
void *threadWork(void *args);

void tpoolInit(tpool_t *tpool, int poolSize) {
    assert(tpool);
    assert(poolSize > 0);

    // initilize the struct
    tpool->threads = malloc(sizeof(pthread_t) * poolSize);
    tpool->poolSize = poolSize;
    tpool->stopWork = false;

    tpool->lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(tpool->lock, NULL);

    tpool->cond = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(tpool->cond, NULL);

    tpool->queue = malloc(sizeof(Queue));
    queueInit(tpool->queue, sizeof(tpool_args));

    // put the threads to work;
    for (int i = 0; i < poolSize; i++) 
        pthread_create(&(tpool->threads[i]), NULL, threadWork, tpool);
}

void *threadWork(void *args) {
    tpool_t *tpool = (tpool_t *)args;

    while (true) {
        pthread_mutex_lock(tpool->lock);

        while (tpool->queue->size == 0 && !tpool->stopWork) 
            pthread_cond_wait(tpool->cond, tpool->lock);

        if (tpool->stopWork && tpool->queue->size == 0) {
            pthread_mutex_unlock(tpool->lock);
            return NULL;
        }

        tpool_args workInfo;
        deQueue(tpool->queue, &workInfo);
        Func func = workInfo.func;
        void *funcArgs = workInfo.args;

        pthread_mutex_unlock(tpool->lock);

        func(funcArgs);
    }
    
    return NULL;
}

void tpoolSchedule(tpool_t *tpool, Func func, void *args) {
    assert(func);

    tpool_args tpoolArgs;
    tpoolArgs.func = func;
    tpoolArgs.args = args;
    
    pthread_mutex_lock(tpool->lock);
    enQueue(tpool->queue, &tpoolArgs);
    pthread_cond_signal(tpool->cond);
    pthread_mutex_unlock(tpool->lock);
}

void tpoolDestroy(tpool_t *tpool) {
    pthread_mutex_lock(tpool->lock);

    tpool->stopWork = true;

    pthread_cond_broadcast(tpool->cond);
    pthread_mutex_unlock(tpool->lock);


    for (int i = 0; i < tpool->poolSize; i++) 
        pthread_join(tpool->threads[i], NULL);

    queueDestroy(tpool->queue);
    free(tpool->queue);
    pthread_cond_destroy(tpool->cond);
    free(tpool->cond);
    pthread_mutex_destroy(tpool->lock);
    free(tpool->lock);
    free(tpool->threads);
}