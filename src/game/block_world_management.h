#pragma once
#include "math/vector.h"
#include <stddef.h>

#define NUM_WORLD_QUEUE_ITEMS 128

typedef struct {
    u16 chunk_index;
    vec3_s32_t position;
} block_chunk_update_t;

extern size_t num_procedural_generate_queue_items;
extern block_chunk_update_t procedural_generate_queue[NUM_WORLD_QUEUE_ITEMS];

extern size_t num_visuals_update_queue_items;
extern block_chunk_update_t visuals_update_queue[NUM_WORLD_QUEUE_ITEMS];

void manage_block_world(vec3_s32_t center_pos);
void handle_world_queues(void);