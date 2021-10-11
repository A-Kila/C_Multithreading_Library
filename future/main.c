#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "future.h"

#define ASSERT(cond, print) if (!(cond)) {\
            printf print;\
            futureDestroy(&fut);\
            return -1;\
        }


void testIntFunc(void *args, void * ret) {
    *(int *)ret = *(int *)args;
    *(int *)ret *= 5;
}

int testInt() {
    future_t fut;
    for (int i = 0; i < 100000; i++) {
        futureCreate(&fut, testIntFunc, &i, sizeof(int));

        int ret;
        futureGet(&fut, &ret);
        ASSERT(ret == 5 * i, ("testInt Failed --- %d != %d\n", ret, i));

        futureDestroy(&fut);
    }
    printf("TestInt Complete\n");
    return 0;
}

void testDoubleFunc(void *args, void *ret) {
    *(double *)ret = *(double *)args;
    *(double *)ret /= 1564;
}

int testDouble() {
    future_t fut;
    for (int i = 0; i < 100000; i++) {
        double doub = ((double)rand() / RAND_MAX) * rand();
        futureCreate(&fut, testDoubleFunc, &doub, sizeof(double));

        double ret;
        futureGet(&fut, &ret);
        ASSERT(ret == (doub / 1564), ("testDouble Failed --- %f != %f\n", ret, doub));

        futureDestroy(&fut);
    }
    printf("TestDouble Complete\n");
    return 0;
}

void testStringFunc(void *args, void *ret) {
    char *copy = malloc(1001);
    strcpy(copy, *(char **)args);
    *(char **)ret = copy;
}

int testString() {
    future_t fut;
    char string[1001];
    char *s = string;
    s[1000] = '\0';
    for (int i = 0; i < 1000; i++) {
        char c = rand() / _SC_CHAR_MAX;
        while (c == '\0') c = rand() / _SC_CHAR_MAX;
        if (c < 0) c = -c;
        s[i] = c;

        futureCreate(&fut, testStringFunc, &s, sizeof(char *));

        char *ret;
        futureGet(&fut, &ret);
        ASSERT(!strcmp(ret, s), ("testString Failed --- %s != %s\n", ret, s));

        free(ret);
        futureDestroy(&fut);
    }
    printf("TestString Complete\n");
    return 0;
}

typedef struct {
    chan_t chanDummy;
    pthread_t threadDummy;
    int test;
    pthread_mutex_t mutexDummy;
    pthread_cond_t condDummy;
} strc;

void testStructFunc(void *args, void *ret) {
    strc *data = args;
    chanInit(&(data->chanDummy), 4, 4);
    data->test = 1121;

    strc copy = *data;
    memcpy(ret, &copy, sizeof(strc));
}

int testStruct() {
    future_t fut;
    strc src;
    strc copy;

    futureCreate(&fut, testStructFunc, &src, sizeof(strc));

    futureGet(&fut, &copy);
    ASSERT(src.test == copy.test && src.chanDummy.elemSize == copy.chanDummy.elemSize,
           ("testStruct Failed --- %p != %p\n", &copy, &src));

    chanDestroy(&(copy.chanDummy));
    futureDestroy(&fut);
    
    printf("TestStruct Complete\n");
    return 0;
}

int main() {
    srand(time(0));
    if (testInt() || testDouble() ||
        testString() || testStruct())
    {
        return -1;
    }

    printf("Test Complete\n");
    return 0;
}