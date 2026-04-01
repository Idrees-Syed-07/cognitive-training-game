#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//creating node with position in graph and position in gui
static Node *create_node(int graph_position[2], int visual_position[2]) {
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

//linking two nodes
static void link_nodes(Node *a, Node *b, Direction direction) {
    if (a == NULL || b == NULL) return;

    a->neighbours[direction] = b;
    b->neighbours[(direction + 2) % 4] = a; //opposite direction
}

//unlinking two nodes
static void unlink_nodes(Node *a, Node *b) {
    if (a == NULL || b == NULL) return;

    //checking which direction nodes are linked
    for (int i = 0; i < 4; i++) {
        if (a->neighbours[i] == b) {
            a->neighbours[i] = NULL; //unlinking
            b->neighbours[(i + 2) % 4] = NULL; //opposite direction

            break;
        }
    }
}

//depth first search
static void dfs(Node *node, bool visited[GRID_SIZE][GRID_SIZE]) {
    //marking node as visited
    visited[node->graph_position[0]][node->graph_position[1]] = true;

    //visiting neighbours
    for (int i = 0; i < 4; i++) {
        Node *n = node->neighbours[i];
        if (n != NULL && !visited[n->graph_position[0]][n->graph_position[1]]) dfs(n, visited);
    }
}

//checking if all nodes in graph are reachable, returns true if so and false if not
static bool is_graph_connected(Graph *graph) {
    bool visited[GRID_SIZE][GRID_SIZE] = {{false}}; //array keeps track of all nodes visited

    //starting depth first search from first node
    dfs(graph->nodes[0][0], visited);

    //checking if all nodes were visited
    for (int row = 0; row < GRID_SIZE; row++)
        for (int col = 0; col < GRID_SIZE; col++)
            if (!visited[row][col]) return false;

    return true;
}

//freeing all nodes in graph and itself
void free_graph(Graph *graph) {
    if (graph == NULL) return;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            free(graph->nodes[i][j]);
        }
    }

    free(graph);
}

//generating nodes with random visual positions
static void generate_nodes(Graph *graph) {
    int positions[GRID_SIZE * GRID_SIZE][2]; //array of xy coordinates
    int index = 0;

    //filling array will all possible coordinates (0,0), (0,1), ..., (1, 0), (1,1), ...
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            positions[index][0] = i;
            positions[index][1] = j;

            index++;
        }
    }

    //randomly shuffling visual positions in the array using Fisher-Yates algorithm
    for (int i = (GRID_SIZE * GRID_SIZE) - 1; i > 0; i--) { 
        int random = rand() % (i + 1); //get random visual position in array
        int temp[2] = {positions[i][0], positions[i][1]};

        //swapping current visual position with random one
        for (int j = 0; j < 2; j++) {
            positions[i][j] = positions[random][j];
            positions[random][j] = temp[j];
        }
    }

    //creating actual nodes in the graph with the random visual positions
    index = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            graph->nodes[i][j] = create_node((int[]) {i, j}, (int[]) {positions[index][0], positions[index][1]});
            index++;
        }
    }
}

//connecting nodes in the graph by up right down and left
static void connect_graph(Graph *graph) {
    //horizontal edges
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            link_nodes(graph->nodes[i][j], graph->nodes[i][j + 1], RIGHT);
        }
    }

    //vertical edges
    for (int i = 0; i < GRID_SIZE - 1; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            link_nodes(graph->nodes[i][j], graph->nodes[i + 1][j], DOWN);
        }
    }
}

//randomly adding coins in graph
static void place_coins(Graph *graph) {
    int placed = 0;

    while (placed < COIN_COUNT) {
        //getting random position
        int row = rand() % GRID_SIZE;
        int col = rand() % GRID_SIZE;

        //placing if no coin already
        if (!graph->nodes[row][col]->has_coin) {
            graph->nodes[row][col]->has_coin = true;
            placed++;
        }
    }
}

//removing edges in the graph to create invisible walls, generating a maze
static void add_walls(Graph *graph) {
    //this struct stores the positions of two nodes connected by an edge
    typedef struct {
        int a_row; 
        int a_col;
        int b_row;
        int b_col;
    } Edge;

    //creating array to hold edges
    int total_edges = (GRID_SIZE - 1) * GRID_SIZE * 2;
    Edge *edges = malloc(sizeof(Edge) * total_edges);
    int edge_count = 0;

    //filling array with horizontal edges
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            edges[edge_count] = (Edge){i, j, i, j + 1};
            edge_count++;
        }
    }

    //filling array with vertical edges
    for (int i = 0; i < GRID_SIZE - 1; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            edges[edge_count] = (Edge){i, j, i + 1, j};
            edge_count++;
        }
    }

    //shuffling edges in the array using Fisher-Yates algorithm
    for (int i = edge_count - 1; i > 0; i--) {
        //getting random edge in array
        int random = rand() % (i + 1); 
        Edge temp = edges[i];
        //swapping current edge with random one
        edges[i] = edges[random];
        edges[random] = temp;
    }

    int removed = 0;
    for (int i = 0; i < edge_count; i++) {
        if (removed == WALL_COUNT) break;

        //getting the nodes connected by the random edge
        Node *a = graph->nodes[edges[i].a_row][edges[i].a_col];
        Node *b = graph->nodes[edges[i].b_row][edges[i].b_col];

        unlink_nodes(a, b);

        //incrementing removed counter if graph still connected and relinking if not
        if (is_graph_connected(graph)) removed++;
        else link_nodes(a, b, edges[i].a_col == edges[i].b_col ? DOWN : RIGHT);
    }

    free(edges);
}

//creates a graph and generates a maze in it
void generate_maze(Graph *graph) {
    srand((unsigned)time(NULL)); //getting seed for random numbers

    generate_nodes(graph);
    connect_graph(graph);
    place_coins(graph);
    add_walls(graph);
}