#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

#define GRID_SIZE 5
#define COIN_COUNT 3
#define WALL_COUNT 10

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT
} Direction;

typedef struct Node {
    int graph_position[2], visual_position[2];
    bool has_coin;
    struct Node* neighbours[4];
} Node;

typedef struct {
    Node *nodes[GRID_SIZE][GRID_SIZE];
} Graph;

void freeGraph(Graph *graph);
void generateMaze(Graph* graph);

#endif
