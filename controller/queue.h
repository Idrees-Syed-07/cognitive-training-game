#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#define CAPACITY 256

enum {X, Y, Z};

typedef struct {
    float data[CAPACITY][3];
    int front;
    int rear;
    int count;
} Queue;

Queue* createQueue();
bool isEmpty(const Queue *q);
void enqueue(Queue *q, float x, float y, float z);
bool dequeue(Queue *q, float out[3]);

#endif
