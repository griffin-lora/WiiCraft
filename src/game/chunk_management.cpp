#include "chunk_management.hpp"
#include "camera.hpp"
#include "chunk_core.hpp"
#include "math.hpp"
#include "stored_chunk.hpp"
#include "util.hpp"

using namespace game;

void game::manage_chunks_around_camera(
    s32 chunk_erasure_radius,
    s32 chunk_generation_radius,
    math::matrix view,
    const camera& cam,
    std::optional<math::vector3s32>& last_cam_chunk_pos,
    chunk::map& chunks,
    stored_chunk::map& stored_chunks,
    std::vector<math::vector3s32>& inserted_chunk_positions
) {
    auto cam_chunk_pos = game::get_chunk_position_from_world_position(cam.position);

    // Only manage chunk generation if the camera has moved to a new chunk.
    if (!last_cam_chunk_pos.has_value() || cam_chunk_pos != last_cam_chunk_pos) {
        // Remove chunks outside of the sphere of radius chunk_erasure_radius
        for (auto it = chunks.begin(); it != chunks.end();) {
            auto pos = it->first;
            auto& chunk = it->second;
            if (math::squared_length(pos - cam_chunk_pos) > (chunk_erasure_radius * chunk_erasure_radius)) {
                if (chunk.modified) {
                    // If the chunk is modified the chunk in the stored_chunks map
                    stored_chunks.insert(std::make_pair<math::vector3s32, game::stored_chunk>(std::move(pos), { .blocks = std::move(chunk.blocks) }));
                }
                // Notify neighbor chunks that they need to update their neighborhood to avoid a dangling reference
                game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk);
                it = chunks.erase(it);
            } else {
                ++it;
            }
        }

        // Generate chunks around the sphere of radius chunk_generation_radius
        game::iterate_positions_in_sphere(chunk_generation_radius, [view, &chunks, &inserted_chunk_positions, &cam_chunk_pos](auto& offset) {
            auto chunk_pos = cam_chunk_pos + offset;
            if (!chunks.count(chunk_pos)) {
                inserted_chunk_positions.push_back(chunk_pos);
                // Compiler was complaining that chunk_pos wasn't an rvalue so I casted it. Just don't use it after this.
                chunks.insert(std::make_pair<math::vector3s32, game::chunk>(std::move(chunk_pos), { view, chunk_pos }));
            }
        });
        
        for (auto pos : inserted_chunk_positions) {
            auto& chunk = chunks.at(pos);

            if (stored_chunks.count(pos)) {
                auto& stored_chunk = stored_chunks.at(pos);
                chunk.blocks = std::move(stored_chunk.blocks);
                stored_chunks.erase(pos);
            } else {
                chunk.blocks.resize_without_copying(game::chunk::BLOCKS_COUNT);
                game::generate_blocks(chunk, pos, 100);
            }

            game::update_chunk_neighborhood(chunks, pos, chunk);
            // Notify old chunks that there is a new chunk neighboring them
            game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk);
        }

        // Clear the update neighborhood flag that was set before
        for (auto pos : inserted_chunk_positions) {
            auto& chunk = chunks.at(pos);
            chunk.update_neighborhood = false;
        }

        inserted_chunk_positions.clear();

        last_cam_chunk_pos = cam_chunk_pos;
    }
}