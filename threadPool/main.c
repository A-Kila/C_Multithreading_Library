#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "threadPool.h"

#define ASSERT(cond, print) if (!(cond)) {\
            printf print;\
            exit(0);\
        }

#define POOLSIZE 100
#define N_FUNCS 1000000

typedef struct {
    pthread_mutex_t *lock;
    int *n;
} testStruct;

void* testFunc(void *args) {
    testStruct *strc = args;
    int i;
    for (i = 0; i < 5; i++);
    pthread_mutex_lock(strc->lock);
    *(strc->n) += i;
    pthread_mutex_unlock(strc->lock);
}

int main() {
    tpool_t tpool;
    tpoolInit(&tpool, POOLSIZE);

    int num = 0;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    printf("Starting thread pool test\n");

    for (int i = 0; i < N_FUNCS; i++) {
        testStruct args = { &lock, &num };
        tpoolSchedule(&tpool, testFunc, &args);
    }
    tpoolDestroy(&tpool);

    ASSERT(num == N_FUNCS * 5, ("!!!Test failed --- %d != %d", num, N_FUNCS * 5));
    
    printf("Test pased!\n");

    return 0;
}