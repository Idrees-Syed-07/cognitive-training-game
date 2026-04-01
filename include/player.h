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

Player *create_player(Graph *maze);
MoveResult move_player(Player *player, Direction direction);

#endif
