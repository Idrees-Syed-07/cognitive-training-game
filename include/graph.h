#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

#define GRID_SIZE 5
#define COIN_COUNT 3

typedef struct Node {
    int row;
    int col;
    int x;
    int y;
    bool has_coin;
    struct Node *up;
    struct Node *down;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node *nodes[GRID_SIZE][GRID_SIZE];
} Graph;

Node *node_create(int row, int col, int x, int y, bool has_coin);
void node_link(Node *a, Node *b, char direction);
void node_unlink(Node *a, Node *b);
void node_free(Node *node);

Graph *graph_create(int num_disconnections);
void graph_print(Graph *graph);
bool graph_is_connected(Graph *graph);
void graph_free(Graph *graph);

#endif
