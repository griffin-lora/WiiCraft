#pragma once
#include "pool.h"

void update_block_chunk_visuals(
    vec3s world_pos,
    block_display_list_chunk_descriptor_t descriptors[],
    const block_type_t block_types[],
    const block_type_t front_block_types[],
    const block_type_t back_block_types[],
    const block_type_t top_block_types[],
    const block_type_t bottom_block_types[],
    const block_type_t right_block_types[],
    const block_type_t left_block_types[]
);