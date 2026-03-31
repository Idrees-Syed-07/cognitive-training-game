#include "queue.h"
#include <stdlib.h>

Queue* createQueue() {
    Queue *q = malloc(sizeof(Queue));

    q->front = 0;
    q->rear = -1;
    q->count = 0;

    return q;
}

bool isEmpty(Queue *q) {
    return q->count == 0;
}

void enqueue(Queue *q, float x, float y, float z) {
    q->rear = (q->rear + 1) % CAPACITY;
    
    q->data[q->rear][X] = x;
    q->data[q->rear][Y] = y;
    q->data[q->rear][Z] = z;

    if (q->count == CAPACITY) q->front = (q->front + 1) % CAPACITY;
    else q->count++;
}

bool dequeue(Queue *q, float out[3]) {
    if (isEmpty(q)) return false;

    for (int i = 0; i < 3; i++) {
        out[i] = q->data[q->front][i];
    }
    
    q->front = (q->front + 1) % CAPACITY;
    q->count--;

    return true;
}