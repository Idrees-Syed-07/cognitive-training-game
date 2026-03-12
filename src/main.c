#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "gui.h"

static int select_difficulty(void) {
    printf("Select difficulty: (1) easy, (2) medium or (3) hard\n");

    int choice;
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) return -1;

    switch (choice) {
        case 1:  return 4;
        case 2:  return 8;
        case 3:  return 12;
        default: return -1;
    }
}

int main(int argc, char *argv[]) {
    int disconnections = select_difficulty();
    if (disconnections < 0) {
        printf("Invalid choice.\n");
        return EXIT_FAILURE;
    }

    Graph *graph = graph_create(disconnections);
    if (!graph) {
        fprintf(stderr, "Failed to create graph.\n");
        return EXIT_FAILURE;
    }

    int status = gui_run(graph, argc, argv);

    graph_free(graph);
    return status;
}
