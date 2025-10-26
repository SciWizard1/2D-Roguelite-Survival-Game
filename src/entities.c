#include "game.h"

uint32_t *entity_ids = NULL;
int32_t *entity_pos_x = NULL;
int32_t *entity_pos_y = NULL;

uint32_t current_entities = 0;
uint32_t entity_pool_size = 0;
uint32_t new_entity_pool_size = 0;

uint32_t player_index = 0; // Index of the entity that the player is represented by.

void resize_entity_array() {
    entity_ids   = tracked_realloc(entity_ids  , new_entity_pool_size * sizeof(uint32_t));
    entity_pos_x = tracked_realloc(entity_pos_x, new_entity_pool_size * sizeof(uint32_t));
    entity_pos_y = tracked_realloc(entity_pos_y, new_entity_pool_size * sizeof(uint32_t));
    entity_pool_size = new_entity_pool_size;
}
