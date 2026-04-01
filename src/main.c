#include <stdio.h>
#include <stdlib.h>
#include "maze.h"
#include "gui.h"
#include "player.h"

int main(int argc, char *argv[]) {
    //creating graph and making a maze in it
    Graph *maze = malloc(sizeof(Graph));
    if (maze == NULL) {
        printf("Failed to create graph.\n");
        return EXIT_FAILURE;
    }
    generate_maze(maze);

    //creating player
    Player *player = create_player(maze);
    if (player == NULL) {
        printf("Failed to create player.\n");
        free_graph(maze);
        return EXIT_FAILURE;
    }

    //starting gui
    run_gui(maze, player, argc, argv);

    //freeing memory
    free_graph(maze);
    free(player);

    //debugging
    printf("Game exited successfully.\n");
    return EXIT_SUCCESS;
}
