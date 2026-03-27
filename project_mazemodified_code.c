#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Node *createNode(int graph_position[2], int visual_position[2]) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;

    node->graph_position[0] = graph_position[0];
    node->visual_position[1] = visual_position[1];
    node->graph_position[0] = graph_position[0];
    node->visual_position[1] = visual_position[1];

    node->has_coin = false;
      //no initial neighbours
    node->neighbours[0] = NULL;
    node->neighbours[1] = NULL;
    node->neighbours[2] = NULL;
    node->neighbours[3] = NULL;
    return node;
}
  //connects two nodes in a dirrection
static void linkNodes(Node *a, Node *b, Direction direction) {
    if (a == NULL || b == NULL) return;

    a->neighbours[direction] = b;
    b->neighbours[(direction + 2) % 4] = a; //opposite dirr'n
}
 //removes node connections
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
//marks accessible nodes nearby
static void dfs(Node *node, bool visited[GRID_SIZE][GRID_SIZE]) {
    int row = node->graph_position[0];
    int col = node->graph_position[1];
    visited[row][col] = true;

    for (int i = 0; i < 4; i++) {
        Node *n = node->neighbours[i];
       if (n == NULL) continue;
        int nrow = n->graph_position[0];
        int ncol = n->graph_position[1];
        if (visited[nrow][ncol] == 0) dfs(n, visited);
    }
}

static bool isGraphConnected(Graph *graph) {
    bool visited[GRID_SIZE][GRID_SIZE] = {0};

    dfs(graph->nodes[0][0], visited);

    for (int row = 0; row < GRID_SIZE; row++)
        for (int col = 0; col < GRID_SIZE; col++)
            if (visited[row][col] == 0) return false;

    return true;
}
 //frees all nodes and graph
void freeGraph(Graph *graph) {
    if (graph == NULL) return;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            free(graph->nodes[i][j]);
        }
    }

    free(graph);
}
//creates all nodes with a random position in the graph
static void generateNodes(Graph *graph) {
    int positions[GRID_SIZE * GRID_SIZE][2];
    int index = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            positions[index][0] = i;
            positions[index][1] = j;
             //fills in the position array with (x, y) coords
            index++;
        }
    }
         //shuffle it
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
            graph->nodes[i][j] = createNode(
                (int[]){i, j},
                (int[]){positions[index][0], positions[index][1]}
            );
            index++;
        }
    }
}
//links adjacent nodes to the graph is full
static void connectGraph(Graph *graph) {
    //Horizontal links
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            linkNodes(graph->nodes[i][j], graph->nodes[i][j + 1], RIGHT);
        }
    }
    //Vertical links
    for (int i = 0; i < GRID_SIZE - 1; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            linkNodes(graph->nodes[i][j], graph->nodes[i + 1][j], DOWN);
        }
    }
}
//random coin placement
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
//disconnects some nodes to act as "walls"
static void addWalls(Graph *graph) {
    typedef struct {
       int a_row, a_col, b_row, b_col;
    } Edge;

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
        Edge temp = edges[i]; //shuffle edges
        edges[i] = edges[random];
        edges[random] = temp;
    }

    int removed = 0; //ensures graph is dissconnected in the wall creation process
    for (int i = 0; i < edge_count && removed < WALL_COUNT; i++) {
        Node *a = graph->nodes[edges[i].a_row][edges[i].a_col];
        Node *b = graph->nodes[edges[i].b_row][edges[i].b_col];

        unlinkNodes(a, b);

        if (isGraphConnected(graph)) removed++;
        else linkNodes(a, b, edges[i].a_col == edges[i].b_col ? DOWN : RIGHT);
    }

    free(edges);
}
//runs all the steps to create the maze
void generateMaze(Graph *graph) {
    srand((unsigned)time(NULL));

    generateNodes(graph);
    connectGraph(graph);
    placeCoins(graph);
    addWalls(graph);
}