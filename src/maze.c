#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static Node *createNode(int graph_position[2], int visual_position[2]) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;

    for (int i = 0; i < 2; i++) {
        node->graph_position[i] = graph_position[i];
        node->visual_position[i] = visual_position[i];
    }

    node->has_coin = false;

    for (int i = 0; i < 4; i++) {
        node->neighbours[i] = NULL;
    }

    return node;
}

static void linkNodes(Node *a, Node *b, Direction direction) {
    if (a == NULL || b == NULL) return;

    a->neighbours[direction] = b;
    b->neighbours[(direction + 2) % 4] = a; 
}

static void unlinkNodes(Node *a, Node *b) {
    if (a == NULL || b == NULL) return;

    for (int i = 0; i < 4; i++) {
        if (a->neighbours[i] == b) {
            a->neighbours[i] = NULL;
            b->neighbours[(i + 2) % 4] = NULL;

            break;
        }
    }
}

static void dfs(Node *node, bool visited[GRID_SIZE][GRID_SIZE]) {
    visited[node->graph_position[0]][node->graph_position[1]] = true;

    for (int i = 0; i < 4; i++) {
        Node *n = node->neighbours[i];
        if (n != NULL && !visited[n->graph_position[0]][n->graph_position[1]]) dfs(n, visited);
    }
}

static bool isGraphConnected(Graph *graph) {
    bool visited[GRID_SIZE][GRID_SIZE] = {{false}};

    dfs(graph->nodes[0][0], visited);

    for (int row = 0; row < GRID_SIZE; row++)
        for (int col = 0; col < GRID_SIZE; col++)
            if (!visited[row][col]) return false;

    return true;
}

void freeGraph(Graph *graph) {
    if (graph == NULL) return;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            free(graph->nodes[i][j]);
        }
    }

    free(graph);
}

static void generateNodes(Graph *graph) {
    int positions[GRID_SIZE * GRID_SIZE][2];
    int index = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            positions[index][0] = i;
            positions[index][1] = j;

            index++;
        }
    }

    for (int i = (GRID_SIZE * GRID_SIZE) - 1; i > 0; i--) {
        int random = rand() % (i + 1);
        int temp[2] = {positions[i][0], positions[i][1]};

        for (int j = 0; j < 2; j++) {
            positions[i][j] = positions[random][j];
            positions[random][j] = temp[j];
        }
    }

    index = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            graph->nodes[i][j] = createNode((int[]){i, j}, (int[]){positions[index][0], positions[index][1]});
            index++;
        }
    }
}

static void connectGraph(Graph *graph) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            linkNodes(graph->nodes[i][j], graph->nodes[i][j + 1], RIGHT);
        }
    }

    for (int i = 0; i < GRID_SIZE - 1; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            linkNodes(graph->nodes[i][j], graph->nodes[i + 1][j], DOWN);
        }
    }
}

static void placeCoins(Graph *graph) {
    int placed = 0;

    while (placed < COIN_COUNT) {
        int row = rand() % GRID_SIZE;
        int col = rand() % GRID_SIZE;

        if (!graph->nodes[row][col]->has_coin) {
            graph->nodes[row][col]->has_coin = true;
            placed++;
        }
    }
}

static void addWalls(Graph *graph) {
    typedef struct {int a_row, a_col, b_row, b_col;} Edge;

    int total_edges = (GRID_SIZE - 1) * GRID_SIZE * 2;
    Edge *edges = malloc(sizeof(Edge) * total_edges);
    int edge_count = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            edges[edge_count] = (Edge){i, j, i, j + 1};
            edge_count++;
        }
    }

    for (int i = 0; i < GRID_SIZE - 1; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            edges[edge_count] = (Edge){i, j, i + 1, j};
            edge_count++;
        }
    }

    for (int i = edge_count - 1; i > 0; i--) {
        int random = rand() % (i + 1);
        Edge temp = edges[i];
        edges[i] = edges[random];
        edges[random] = temp;
    }

    int removed = 0;
    for (int i = 0; i < edge_count && removed < WALL_COUNT; i++) {
        Node *a = graph->nodes[edges[i].a_row][edges[i].a_col];
        Node *b = graph->nodes[edges[i].b_row][edges[i].b_col];

        unlinkNodes(a, b);

        if (isGraphConnected(graph)) removed++;
        else linkNodes(a, b, edges[i].a_col == edges[i].b_col ? DOWN : RIGHT);
    }

    free(edges);
}

void generateMaze(Graph *graph) {
    srand((unsigned)time(NULL));

    generateNodes(graph);
    connectGraph(graph);
    placeCoins(graph);
    addWalls(graph);
}