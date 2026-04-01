#include <stdlib.h>
#include "player.h"

Player *create_player(Graph *maze) {
    Player *player = malloc(sizeof(Player));
    if (!player) return NULL;

    player->coins_left = COIN_COUNT;

    //placing player on first node without coin
    player->node = NULL;
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (!maze->nodes[row][col]->has_coin) {
                player->node = maze->nodes[row][col];
                break;
            }
        }

        if (player->node != NULL) break;
    }

    return player;
}

MoveResult move_player(Player *player, Direction direction) {
    Node *next = player->node->neighbours[direction];
    if (next == NULL) return MOVE_WALL; //if no connection there is wall

    player->node = next;

    //decreasing number of coins left if user moves to node with coin
    if (next->has_coin) {
        next->has_coin = false;
        player->coins_left--;
        if (player->coins_left == 0) return MOVE_WIN; //if this was last coin
    }

    return MOVE_OK; //normal move
}