#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "gui.h"

int main(int argc, char *argv[]) {
    Graph *maze = malloc(sizeof(Graph));
    if (maze == NULL) {
        printf("Failed to create graph.\n");
        return EXIT_FAILURE;
    }

    generateMaze(maze);

    int status = gui_run(maze, argc, argv);

    freeGraph(maze);
    return status;
}
