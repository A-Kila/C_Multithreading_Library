#include <stdlib.h>
#include <assert.h>

#include "future.h"

typedef struct {
    Func func;
    void *args;
    int retSize;
    chan_t *chan;
} future_args;

void *futureHelper(void *args);

void futureCreate(future_t *fut, Func func, void *args, int retSize) {
    assert(fut);
    assert(func);

    // initialization
    future_args *helperArgs = malloc(sizeof(future_args));
    helperArgs->retSize = retSize;
    fut->func = helperArgs->func = func;
    fut->args = helperArgs->args = args;
    fut->isCalled = false;
    
    fut->chan = malloc(sizeof(chan_t));
    chanInit(fut->chan, retSize, 0);
    helperArgs->chan = fut->chan;

    //creating a thread
    pthread_create(&(fut->thread), NULL, futureHelper, helperArgs);
}

void *futureHelper(void *args) {
    future_args *data = args;
    void *ret = malloc(data->retSize);

    (data->func)(data->args, ret);
    chanSend(data->chan, ret);
    
    free(ret);
    free(data);
    return NULL;
}

void futureGet(future_t *fut, void *elem) {
    assert(!(fut->isCalled));
    assert(elem != NULL);

    chanRecv(fut->chan, elem);
    fut->isCalled = true;
}

void futureDestroy(future_t *fut) {
    pthread_join(fut->thread, NULL);

    chanDestroy(fut->chan);
    free(fut->chan);
}