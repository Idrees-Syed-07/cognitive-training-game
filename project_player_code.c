#include <stdlib.h>
#include "player.h"

Player *player_create(Graph *graph) {
    Player *player = malloc(sizeof(Player));
    if (player == NULL) return NULL;

    player->coins_left = COIN_COUNT;

         // Find a starting node that doesn't have a coin
    player->node = NULL;
    for (int row = 0; row < GRID_SIZE && found ==0; row++) {
        for (int col = 0; col < GRID_SIZE && found ==0; col++) {
            if (graph->nodes[row][col]->has_coin ==0) {
                player->node = graph->nodes[row][col];
                found = 1; //Condition to break both loops
            }
        }
    }

    return player;
}

MoveResult player_move(Player *player, char direction) {
    Node *next = NULL;
     //The possible difrrections that the player can take
    switch (direction) {
        case 'w': next = player->node->neighbours[UP]; break;
        case 's': next = player->node->neighbours[DOWN]; break;
        case 'a': next = player->node->neighbours[LEFT]; break;
        case 'd': next = player->node->neighbours[RIGHT]; break;
    }

    if (next == NULL) return MOVE_WALL;

    player->node = next;
    // Cases for if the player runs into a wall or a coin
    if (next->has_coin)return MOVE_OK;

    next->has_coin = false;
    player->coins_left--;

    if (player->coins_left == 0) return MOVE_WIN; return MOVE_OK;

    

}

void player_free(Player *player) {
    free(player);
}