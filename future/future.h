#include <stdbool.h>
#include <pthread.h>

#include "../channel/channel.h"

typedef void (*Func)(void *, void *);

typedef struct {
    Func func;
    void *args;
    pthread_t thread;
    chan_t *chan;
    bool isCalled;
} future_t;


// Stores the return value of a func in future 
void futureCreate(future_t *fut, Func func, void *args, int retSize);

/*
 * Function: futureGet
 * -------------------
 * Returns the stored function return value.
 * Can be called only once.
 */
void futureGet(future_t *fut, void *elem);

void futureDestroy(future_t *fut);