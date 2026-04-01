// IMPLEMENTATION OF A CIRCULAR QUEUE THAT AUTOMATICALLY OVERWRITES OLD DATA //

#include "queue.h"
#include <stdlib.h>

//allocating memory for queue and setting up start values
Queue* createQueue() {
    Queue *q = (Queue *) malloc(sizeof(Queue));

    q->front = 0;
    q->rear = -1;
    q->count = 0;

    return q;
}

bool isEmpty(Queue *q) {
    return q->count == 0;
}

//adding element to end of queue, replacing old data automatically
void enqueue(Queue *q, float x, float y, float z) {
    q->rear = (q->rear + 1) % CAPACITY; //going to front if at end of array
    

    //adding data
    q->data[q->rear][X] = x;
    q->data[q->rear][Y] = y;
    q->data[q->rear][Z] = z;

    if (q->count == CAPACITY) q->front = (q->front + 1) % CAPACITY; //moving front up if queue full
    else q->count++; //increasing count if queue not full
}

//removing front element from queue, returns false if queue empty
bool dequeue(Queue *q, float out[3]) {
    if (isEmpty(q)) return false;

    for (int i = 0; i < 3; i++) {
        out[i] = q->data[q->front][i];
    }
    
    q->front = (q->front + 1) % CAPACITY; 
    q->count--;

    return true;
}