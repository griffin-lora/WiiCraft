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
    const math::matrix view,
    const camera& cam,
    std::optional<math::vector3s32>& last_cam_chunk_pos,
    chunk::map& chunks,
    stored_chunk::map& stored_chunks,
    std::vector<math::vector3s32>& chunk_positions_to_erase
) {
    auto cam_chunk_pos = game::get_chunk_position_from_world_position(cam.position);

    // Only erase chunks if the camera has moved to a new chunk.
    if (!last_cam_chunk_pos.has_value() || cam_chunk_pos != last_cam_chunk_pos) {
        // Remove chunks outside of the sphere of radius chunk_erasure_radius
        for (auto& [ pos, chunk ] : chunks) {
            if (math::squared_length(pos - cam_chunk_pos) > (chunk_erasure_radius * chunk_erasure_radius)) {
                if (chunk.modified) {
                    // If the chunk is modified the chunk in the stored_chunks map
                    auto stored_pos = pos;
                    stored_chunks.insert(std::make_pair<math::vector3s32, game::stored_chunk>(std::move(stored_pos), { .blocks = std::move(chunk.blocks) }));
                }
                // Notify neighbor chunks that they need to update their neighborhood to avoid a dangling reference
                game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk);
                // Add to the erasure list
                chunk_positions_to_erase.push_back(pos);
            }
        }

        // Erase all chunks in the erasure list
        for (auto pos : chunk_positions_to_erase) {
            chunks.erase(pos);
        }

        chunk_positions_to_erase.clear();

        last_cam_chunk_pos = cam_chunk_pos;
    }

    // Generate a single chunk around the sphere of radius chunk_generation_radius
    game::iterate_positions_in_sphere(chunk_generation_radius, [view, &chunks, &stored_chunks, &cam_chunk_pos](auto& offset) {
        auto chunk_pos = cam_chunk_pos + offset;
        if (!chunks.contains(chunk_pos)) {
            auto pos = chunk_pos;
            // Compiler was complaining that chunk_pos wasn't an rvalue so I casted it. Just don't use it after this.
            auto [ it, success ] = chunks.insert(std::make_pair<math::vector3s32, game::chunk>(std::move(chunk_pos), { view, pos }));

            auto& chunk = it->second;
            auto stored_chunk_it = stored_chunks.find(pos);
            if (stored_chunk_it != stored_chunks.end()) {
                auto& stored_chunk = stored_chunk_it->second;
                chunk.blocks = std::move(stored_chunk.blocks);
                stored_chunks.erase(pos);
            } else {
                chunk.blocks.resize_without_copying(game::chunk::BLOCKS_COUNT);
                game::generate_blocks(chunk, pos);
            }

            game::update_chunk_neighborhood(chunks, pos, chunk);
            // Notify old chunks that there is a new chunk neighboring them
            game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk);

            chunk.update_mesh_important = true;
            chunk.update_mesh_unimportant = false;
            chunk.update_neighborhood = false;
            return true;
        }
        return false;
    });
}