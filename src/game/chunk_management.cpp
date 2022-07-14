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
    std::vector<math::vector3s32>& chunk_positions_to_erase,
    chunk::pos_set& chunk_positions_to_create_blocks,
    chunk::pos_set& chunk_positions_to_update_neighborhood_and_mesh
) {
    auto cam_chunk_pos = get_chunk_position_from_world_position(cam.position);

    // Only erase chunks if the camera has moved to a new chunk.
    if (!last_cam_chunk_pos.has_value() || cam_chunk_pos != last_cam_chunk_pos) {
        // Remove chunks outside of the sphere of radius chunk_erasure_radius
        for (auto& [ pos, chunk ] : chunks) {
            if (math::length_squared(pos - cam_chunk_pos) > (chunk_erasure_radius * chunk_erasure_radius)) {
                if (chunk.modified) {
                    // If the chunk is modified the chunk in the stored_chunks map
                    auto stored_pos = pos;
                    stored_chunks.insert(std::make_pair<math::vector3s32, stored_chunk>(std::move(stored_pos), { .blocks = std::move(chunk.blocks) }));
                }
                // Notify neighbor chunks that they need to update their neighborhood (but not their mesh) to avoid a dangling reference
                add_chunk_neighborhood_update_to_neighbors(chunk);
                // Add to the erasure list
                chunk_positions_to_erase.push_back(pos);

                chunk_positions_to_create_blocks.erase(pos);
                chunk_positions_to_update_neighborhood_and_mesh.erase(pos);
            }
        }

        // Erase all chunks in the erasure list
        for (auto pos : chunk_positions_to_erase) {
            chunks.erase(pos);
        }

        chunk_positions_to_erase.clear();

        // Add to chunk_positions_to_generate_blocks positions around the sphere of radius chunk_generation_radius
        iterate_positions_in_sphere(chunk_generation_radius, [view, &chunks, &chunk_positions_to_create_blocks, &cam_chunk_pos](auto& offset) {
            auto chunk_pos = cam_chunk_pos + offset;
            if (!chunks.contains(chunk_pos)) {
                chunk_positions_to_create_blocks.insert(chunk_pos);
            }
        });

        last_cam_chunk_pos = cam_chunk_pos;
    }

    if (chunk_positions_to_create_blocks.size() > 0) {
        auto pos_it = chunk_positions_to_create_blocks.begin();

        auto chunk_pos = *pos_it;
        auto& pos = *pos_it;
        
        // Compiler was complaining that chunk_pos wasn't an rvalue so I casted it. Just don't use it after this.
        auto [ chunk_it, success ] = chunks.insert(std::make_pair<math::vector3s32, chunk>(std::move(chunk_pos), { view, pos }));

        auto& chunk = chunk_it->second;
        auto stored_chunk_it = stored_chunks.find(pos);
        if (stored_chunk_it != stored_chunks.end()) {
            auto& stored_chunk = stored_chunk_it->second;
            chunk.blocks = std::move(stored_chunk.blocks);
            stored_chunks.erase(pos);
        } else {
            chunk.blocks.resize_without_copying(chunk::BLOCKS_COUNT);
            generate_blocks(chunk, pos);
        }

        chunk_positions_to_update_neighborhood_and_mesh.insert(pos);
        chunk_positions_to_create_blocks.erase(pos_it);
    }

    // Once all the chunk blocks have been generated/retrieved, update the neighborhood and mesh of all the chunks in the chunk_positions_to_create_blocks set
    if (chunk_positions_to_create_blocks.size() == 0) {
        for (auto& pos : chunk_positions_to_update_neighborhood_and_mesh) {
            auto& chunk = chunks.at(pos);
            
            // Update the neighborhood of the chunk immediately since it will be used to notify its neighbors
            update_chunk_neighborhood(chunks, pos, chunk);
            // Notify old chunks that there is a new neighbor and to update their neighborhood and mesh
            add_chunk_mesh_neighborhood_update_to_neighbors(chunk);
            
            chunk.update_mesh_unimportant = true;
            chunk.update_neighborhood = false; // Don't update neighborhood since we just updated it
        }

        chunk_positions_to_update_neighborhood_and_mesh.clear();
    }
}