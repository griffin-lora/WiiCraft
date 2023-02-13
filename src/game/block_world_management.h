#pragma once
#include "math/vector.h"
#include <stddef.h>

#define NUM_WORLD_QUEUE_ITEMS 1024

extern size_t num_procedural_generate_queue_items;
extern vec3_s32_t procedural_generate_queue[NUM_WORLD_QUEUE_ITEMS];

extern size_t num_visuals_update_queue_items;
extern vec3_s32_t visuals_update_queue[NUM_WORLD_QUEUE_ITEMS];

void init_block_world(vec3_s32_t corner_pos);
void manage_block_world(vec3_s32_t last_corner_pos, vec3_s32_t corner_pos);
void handle_world_queues(vec3_s32_t corner_pos);