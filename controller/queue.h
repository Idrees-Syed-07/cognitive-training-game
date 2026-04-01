#include <stdbool.h>

#define CAPACITY 256

enum {X, Y, Z};

typedef struct {
    float data[CAPACITY][3];
    int front;
    int rear;
    int count;
} Queue;

//this function creates queue in heap and sets up starting values
Queue* createQueue();
//this function returns true if the queue is empty and false if not
bool isEmpty(Queue *q);
//this function adds xyz data to the end of the queue, automatically overwriting old data
void enqueue(Queue *q, float x, float y, float z);
//this function removes the front xyz data in the queue and puts it in out
bool dequeue(Queue *q, float out[3]);
