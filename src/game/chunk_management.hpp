#pragma once
#include "math.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "stored_chunk.hpp"
#include "block_mesh_generation.hpp"
#include "chrono.hpp"

namespace game {
    void update_chunk_neighborhoods(chunk::map& chunks);

    void update_chunk_visuals(block_quad_building_arrays& building_arrays, chunk::map& chunks, chrono::us& total_mesh_gen_time, chrono::us& last_mesh_gen_time, chrono::us now);

    void manage_chunks_around_camera(
        s32 chunk_erasure_radius,
        s32 chunk_generation_radius,
        const math::matrix view,
        const camera& cam,
        std::optional<math::vector3s32>& last_cam_chunk_pos,
        chunk::map& chunks,
        stored_chunk::map& stored_chunks,
        std::vector<math::vector3s32>& chunk_positions_in_function_state,
        chunk::pos_set& chunk_positions_to_create_blocks,
        chunk::pos_set& chunk_positions_to_update_neighborhood_and_mesh,
        chrono::us& total_block_gen_time
    );
}