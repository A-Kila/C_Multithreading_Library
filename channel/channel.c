#include <stdlib.h>
#include <assert.h>

#include  "channel.h"

void chanInit(chan_t *chan, int elemSize, int bufferSize) {
    assert(chan);
    assert(bufferSize >= 0);
    assert(elemSize > 0);

    chan->elemSize = elemSize;
    
    chan->lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(chan->lock, NULL);

    chan->cond = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(chan->cond, NULL);

    chan->sem = malloc(sizeof(sem_t));
    sem_init(chan->sem, 0, bufferSize);

    chan->queue = malloc(sizeof(Queue));
    queueInit(chan->queue, elemSize);
}

void chanSend(chan_t *chan, void *elem) {
    pthread_mutex_lock(chan->lock);
    
    enQueue(chan->queue, elem);

    pthread_cond_signal(chan->cond);
    pthread_mutex_unlock(chan->lock);
    sem_wait(chan->sem);
}

void chanRecv(chan_t *chan, void *elem) {
    assert(elem != NULL);

    pthread_mutex_lock(chan->lock);
    while (chan->queue->size == 0)
        pthread_cond_wait(chan->cond, chan->lock);
    
    deQueue(chan->queue, elem);
    
    pthread_mutex_unlock(chan->lock);
    sem_post(chan->sem);
}

void chanDestroy(chan_t *chan) {
    queueDestroy(chan->queue);
    free(chan->queue);
    sem_destroy(chan->sem);
    free(chan->sem);
    pthread_cond_destroy(chan->cond);
    free(chan->cond);
    pthread_mutex_destroy(chan->lock);
    free(chan->lock);
}