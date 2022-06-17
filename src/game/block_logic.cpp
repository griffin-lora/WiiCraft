#include "block_logic.hpp"
#include "chunk_core.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

std::optional<raycast> game::get_raycast(const camera& cam, chunk::map& chunks) {
    glm::vec3 raycast_pos = cam.position;
    glm::vec3 dir_vec = cam.look * 0.5f;
    std::optional<math::vector3s32> current_chunk_pos = {};
    chunk::opt_ref current_chunk = {};
    for (u8 i = 0; i < 40; i++) {
        auto raycast_chunk_pos = get_chunk_position_from_world_position(raycast_pos);

        if (current_chunk_pos.has_value()) {
            if (raycast_chunk_pos != current_chunk_pos.value()) {
                if (!chunks.count(raycast_chunk_pos)) {
                    break;
                }
                current_chunk_pos = raycast_chunk_pos;
                current_chunk = chunks.at(raycast_chunk_pos);
            }
        } else {
            if (!chunks.count(raycast_chunk_pos)) {
                break;
            }
            current_chunk_pos = raycast_chunk_pos;
            current_chunk = chunks.at(raycast_chunk_pos);
        }

        math::vector3s32 raycast_block_pos = floor_float_position(raycast_pos);
        raycast_block_pos.x = math::mod(raycast_block_pos.x, chunk::SIZE);
        raycast_block_pos.y = math::mod(raycast_block_pos.y, chunk::SIZE);
        raycast_block_pos.z = math::mod(raycast_block_pos.z, chunk::SIZE);
        auto index = game::get_index_from_position(raycast_block_pos);
        auto& block = current_chunk->get().blocks[index];

        if (block.tp != block::type::AIR) {
            return raycast{ .ch_pos = *current_chunk_pos, .ch = *current_chunk, .bl_pos = raycast_chunk_pos, .bl = block };
        }

        raycast_pos += dir_vec;
    }
    return {};
}