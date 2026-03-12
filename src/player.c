#include <stdlib.h>
#include "player.h"

Player *player_create(Graph *graph) {
    Player *player = malloc(sizeof(Player));
    if (!player) return NULL;

    player->coins_left = COIN_COUNT;

    /* Find a starting node that doesn't have a coin */
    player->node = NULL;
    for (int row = 0; row < GRID_SIZE && !player->node; row++) {
        for (int col = 0; col < GRID_SIZE && !player->node; col++) {
            if (!graph->nodes[row][col]->has_coin) {
                player->node = graph->nodes[row][col];
            }
        }
    }

    return player;
}

MoveResult player_move(Player *player, char direction) {
    Node *next = NULL;

    switch (direction) {
        case 'w': next = player->node->up;    break;
        case 's': next = player->node->down;  break;
        case 'a': next = player->node->left;  break;
        case 'd': next = player->node->right; break;
    }

    if (!next) return MOVE_WALL;

    player->node = next;

    if (next->has_coin) {
        next->has_coin = false;
        player->coins_left--;
        if (player->coins_left == 0) return MOVE_WIN;
    }

    return MOVE_OK;
}

void player_free(Player *player) {
    free(player);
}
