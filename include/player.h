#ifndef PLAYER_H
#define PLAYER_H

#include "maze.h"

typedef enum {
    MOVE_OK,
    MOVE_WALL,
    MOVE_WIN
} MoveResult;

typedef struct {
    Node *node;
    int coins_left;
} Player;

//this function creates a player in the given maze, places it on a node without a coin and returns a pointer to it
Player *create_player(Graph *maze);
//this function returns the result of a player trying to move in a given direction
//it also moves the player and updates number of coins left if necessary
MoveResult move_player(Player *player, Direction direction);

#endif
