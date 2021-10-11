#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <semaphore.h>
#include <pthread.h>

#include "../myQueue/myQueue.h"


typedef struct {
    int elemSize;
    sem_t *sem;
    pthread_mutex_t *lock;
    pthread_cond_t *cond;
    Queue *queue;
} chan_t;


// BufferSize = 0 for unbuffered channel
void chanInit(chan_t *chan, int elemSize, int bufferSize);

/*
 * Function: chanSend
 * ------------------
 * Thread Sends a new value to the channel.
 * Works Like FIFO enqueue, but can not push
 * while buffer is full.
 */
void chanSend(chan_t *chan, void *elem);

/*
 * Function: chanRecv
 * ------------------
 * Thread recieves a value from the channel.
 * Works like FIFO dequeue.
 * If the channel is empty, thread waits until
 * other thread sends a value to the channel.
 */
void chanRecv(chan_t *chan, void *elem);

void chanDestroy(chan_t *chan);


#endif