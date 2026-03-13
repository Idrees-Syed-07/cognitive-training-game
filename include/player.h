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

Player *player_create(Graph *graph);
MoveResult player_move(Player *player, char direction);
void player_free(Player *player);

#endif
