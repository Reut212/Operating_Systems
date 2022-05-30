#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifndef QUEUE_H_
#define QUEUE_H_
inline pthread_mutex_t lock;
inline pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


typedef struct QNode {
    void* data;
    struct QNode* next;
    struct QNode* prev;
} node;

typedef struct Queue {
    struct QNode *front, *rear;
} queue;

void* createQ();
void destoryQ(void* q);
void enQ(void* q,void* n);
void* deQ(void* q);

#endif
