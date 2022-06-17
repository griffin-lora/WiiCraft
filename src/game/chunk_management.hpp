#pragma once
#include "math.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "stored_chunk.hpp"
#include "common.hpp"

namespace game {
    void manage_chunks_around_camera(
        s32 chunk_erasure_radius,
        s32 chunk_generation_radius,
        math::matrix view,
        const camera& cam,
        std::optional<math::vector3s32>& last_cam_chunk_pos,
        chunk::map& chunks,
        stored_chunk::map& stored_chunks,
        std::vector<math::vector3s32>& inserted_chunk_positions
    );
};