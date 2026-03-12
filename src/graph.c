#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Node *node_create(int row, int col, int x, int y, bool has_coin) {
    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    node->row = row;
    node->col = col;
    node->x = x;
    node->y = y;
    node->has_coin = has_coin;
    node->up = NULL;
    node->down = NULL;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void node_link(Node *a, Node *b, char direction) {
    if (!a || !b) return;

    switch (direction) {
        case 'u':
            a->up = b;
            b->down = a;
            break;
        case 'd':
            a->down = b;
            b->up = a;
            break;
        case 'l':
            a->left = b;
            b->right = a;
            break;
        case 'r':
            a->right = b;
            b->left = a;
            break;
    }
}

void node_unlink(Node *a, Node *b) {
    if (!a || !b) return;

    if (a->up == b)    a->up = NULL;
    if (a->down == b)  a->down = NULL;
    if (a->left == b)  a->left = NULL;
    if (a->right == b) a->right = NULL;

    if (b->up == a)    b->up = NULL;
    if (b->down == a)  b->down = NULL;
    if (b->left == a)  b->left = NULL;
    if (b->right == a) b->right = NULL;
}

void node_free(Node *node) {
    free(node);
}

static bool bfs_connected(Graph *graph) {
    bool visited[GRID_SIZE][GRID_SIZE] = {{false}};
    Node *queue[GRID_SIZE * GRID_SIZE];
    int front = 0, back = 0;

    queue[back++] = graph->nodes[0][0];
    visited[0][0] = true;
    int count = 1;

    while (front < back) {
        Node *cur = queue[front++];
        Node *neighbors[4] = {cur->up, cur->down, cur->left, cur->right};

        for (int i = 0; i < 4; i++) {
            Node *n = neighbors[i];
            if (n && !visited[n->row][n->col]) {
                visited[n->row][n->col] = true;
                queue[back++] = n;
                count++;
            }
        }
    }

    return count == GRID_SIZE * GRID_SIZE;
}

bool graph_is_connected(Graph *graph) {
    return bfs_connected(graph);
}

Graph *graph_create(int num_disconnections) {
    Graph *graph = malloc(sizeof(Graph));
    if (!graph) return NULL;

    srand((unsigned)time(NULL));

    // Build shuffled coordinate pool
    int total = GRID_SIZE * GRID_SIZE;
    typedef struct { int x, y; } Coord;
    Coord coords[total];

    int idx = 0;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            coords[idx++] = (Coord){x, y};
        }
    }

    // Fisher-Yates shuffle
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Coord tmp = coords[i];
        coords[i] = coords[j];
        coords[j] = tmp;
    }

    // Create all nodes with shuffled coordinates
    idx = 0;
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            graph->nodes[row][col] = node_create(
                row, col, coords[idx].x, coords[idx].y, false
            );
            idx++;
        }
    }

    // Link horizontal edges
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE - 1; col++) {
            node_link(graph->nodes[row][col], graph->nodes[row][col + 1], 'r');
        }
    }

    // Link vertical edges
    for (int row = 0; row < GRID_SIZE - 1; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            node_link(graph->nodes[row][col], graph->nodes[row + 1][col], 'd');
        }
    }

    // Place coins on random nodes
    int coins_placed = 0;
    while (coins_placed < COIN_COUNT) {
        int r = rand() % GRID_SIZE;
        int c = rand() % GRID_SIZE;
        if (!graph->nodes[r][c]->has_coin) {
            graph->nodes[r][c]->has_coin = true;
            coins_placed++;
        }
    }

    // Collect all edges into a list for random removal
    typedef struct { int ar, ac, br, bc; } Edge;
    int total_edges = (GRID_SIZE - 1) * GRID_SIZE * 2;
    Edge *edges = malloc(sizeof(Edge) * total_edges);
    int edge_count = 0;

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE - 1; col++) {
            edges[edge_count++] = (Edge){row, col, row, col + 1};
        }
    }
    for (int row = 0; row < GRID_SIZE - 1; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            edges[edge_count++] = (Edge){row, col, row + 1, col};
        }
    }

    // Shuffle edges
    for (int i = edge_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Edge tmp = edges[i];
        edges[i] = edges[j];
        edges[j] = tmp;
    }

    // Try removing edges while keeping connectivity
    int removed = 0;
    for (int i = 0; i < edge_count && removed < num_disconnections; i++) {
        Node *a = graph->nodes[edges[i].ar][edges[i].ac];
        Node *b = graph->nodes[edges[i].br][edges[i].bc];

        node_unlink(a, b);

        if (bfs_connected(graph)) {
            removed++;
        } else {
            if (edges[i].ac == edges[i].bc) {
                node_link(a, b, 'd');
            } else {
                node_link(a, b, 'r');
            }
        }
    }

    free(edges);
    return graph;
}

void graph_print(Graph *graph) {
    for (int row = 0; row < GRID_SIZE; row++) {
        // Print nodes and horizontal connections
        for (int col = 0; col < GRID_SIZE; col++) {
            Node *n = graph->nodes[row][col];
            if (n->has_coin)
                printf(" C ");
            else
                printf(" . ");

            if (col < GRID_SIZE - 1) {
                if (n->right)
                    printf("--");
                else
                    printf("  ");
            }
        }
        printf("\n");

        // Print vertical connections
        if (row < GRID_SIZE - 1) {
            for (int col = 0; col < GRID_SIZE; col++) {
                Node *n = graph->nodes[row][col];
                if (n->down)
                    printf(" | ");
                else
                    printf("   ");

                if (col < GRID_SIZE - 1)
                    printf("  ");
            }
            printf("\n");
        }
    }
}

void graph_free(Graph *graph) {
    if (!graph) return;

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            node_free(graph->nodes[row][col]);
        }
    }
    free(graph);
}
