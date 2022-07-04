#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "chunk_mesh_generation.hpp"
#include "common.hpp"
#include "glm/gtc/noise.hpp"

using namespace game;
using math::get_noise_at;

static f32 get_biome_value(glm::vec2 position) {
    position /= 256.0f;

    return (
        (get_noise_at(position)) +
        (get_noise_at(position * 3.0f) * 0.5f)
    );
}

static f32 get_plains_height(glm::vec2 position, f32 biome_value) {
    position /= 32.0f;

    return (1.0f - biome_value) * (
        (get_noise_at(position) * 0.1f) +
        (get_noise_at(position * 3.0f) * 0.04f)
    );
}

static f32 get_hills_height(glm::vec2 position, f32 biome_value) {
    position /= 64.0f;

    return biome_value * (
        (get_noise_at(position) * 0.4f) +
        (get_noise_at(position * 3.0f) * 0.2f) +
        (get_noise_at(position * 6.0f) * 0.1f)
    );
}

static f32 get_tallgrass_value(glm::vec2 position) {
    position /= 2.0f;

    return (
        (get_noise_at(position) * 0.5f) + 
        (get_noise_at(position * 2.0f) * 1.0f)
    );
}

void game::generate_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    for (u8 x = 0; x < chunk::SIZE; x++) {
        for (u8 z = 0; z < chunk::SIZE; z++) {
            f32 world_x = game::get_world_coord_from_local_position(x, chunk_pos.x);
            f32 world_z = game::get_world_coord_from_local_position(z, chunk_pos.z);
            glm::vec2 noise_pos = { world_x, world_z };

            auto plains_height = get_noise_at(noise_pos / 32.0f);
            
            auto tallgrass_value = get_tallgrass_value(noise_pos);

            s32 generated_height = (plains_height * 12) + 1;

            for (u8 y = 0; y < chunk::SIZE; y++) {
                auto world_height = game::get_world_coord_from_local_position(y, chunk_pos.y);
                auto index = game::get_index_from_position(math::vector3u8{x, y, z});

                auto& block = chunk.blocks[index];

                if (world_height < generated_height) {
                    if (world_height < (generated_height - 2)) {
                        block = { .tp = block::type::STONE };
                    } else {
                        block = { .tp = block::type::DIRT };
                    }
                } else if (world_height == generated_height) {
                    block = { .tp = block::type::GRASS };
                } else {
                    if (world_height < 5) {
                        block = { .tp = block::type::WATER };
                    } else if (world_height == (generated_height + 1) && tallgrass_value > 0.97f) {
                        block = { .tp = block::type::TALL_GRASS };
                    } else {
                        block = { .tp = block::type::AIR };
                    }
                }
            }
        }
    }
}

template<block::face face>
static chunk::opt_ref get_neighbor_from_map(chunk::map& chunks, const math::vector3s32& pos) {
    math::vector3s32 offset_pos = get_face_offset_position<face>(pos);
    auto it = chunks.find(offset_pos);
    if (it != chunks.end()) {
        return it->second;
    } else {
        return {};
    }
}

void game::update_chunk_neighborhood(chunk::map& chunks, const math::vector3s32& pos, chunk& chunk) {
    chunk.nh = {
        .front = get_neighbor_from_map<block::face::FRONT>(chunks, pos),
        .back = get_neighbor_from_map<block::face::BACK>(chunks, pos),
        .top = get_neighbor_from_map<block::face::TOP>(chunks, pos),
        .bottom = get_neighbor_from_map<block::face::BOTTOM>(chunks, pos),
        .right = get_neighbor_from_map<block::face::RIGHT>(chunks, pos),
        .left = get_neighbor_from_map<block::face::LEFT>(chunks, pos),
    };
}

template<block::face face>
static void add_needed_important_chunk_mesh_update_to_neighbor(chunk& chunk, math::vector3u8 pos) {
    if (is_block_position_at_face_edge<face>(pos)) {
        add_important_chunk_mesh_update_to_neighbor<face>(chunk);
    }
}

void game::add_important_chunk_mesh_update(chunk& chunk, math::vector3u8 pos) {
    chunk.update_mesh_important = true;
    add_needed_important_chunk_mesh_update_to_neighbor<block::face::FRONT>(chunk, pos);
    add_needed_important_chunk_mesh_update_to_neighbor<block::face::BACK>(chunk, pos);
    add_needed_important_chunk_mesh_update_to_neighbor<block::face::TOP>(chunk, pos);
    add_needed_important_chunk_mesh_update_to_neighbor<block::face::BOTTOM>(chunk, pos);
    add_needed_important_chunk_mesh_update_to_neighbor<block::face::RIGHT>(chunk, pos);
    add_needed_important_chunk_mesh_update_to_neighbor<block::face::LEFT>(chunk, pos);
}

template<block::face face>
static void add_chunk_mesh_neighborhood_update_to_neighbor(chunk& chunk) {
    auto nb_chunk_opt = get_neighbor<face>(chunk.nh);
    if (nb_chunk_opt.has_value()) {
        auto& nb_chunk = nb_chunk_opt->get();
        nb_chunk.update_mesh_unimportant = true;
        nb_chunk.update_neighborhood = true;
    }
}

void game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk) {
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::FRONT>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::BACK>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::TOP>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::BOTTOM>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::RIGHT>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::LEFT>(chunk);
}

void game::update_chunks(chunk::map& chunks, ext::data_array<chunk::quad>& building_quads) {
    for (auto& [ pos, chunk ] : chunks) {
        if (chunk.update_neighborhood) {
            chunk.update_neighborhood = false;
            game::update_chunk_neighborhood(chunks, pos, chunk);
        }
    }
    
    bool did_important_mesh_update = false;
    for (auto& [ pos, chunk ] : chunks) {
        if (chunk.update_mesh_important) {
            did_important_mesh_update = true;
            chunk.update_mesh_important = false;
            chunk.update_mesh_unimportant = false;
            game::update_mesh(chunk, building_quads);
        }
    }

    if (!did_important_mesh_update) {
        for (auto& [ pos, chunk ] : chunks) {
            if (chunk.update_mesh_unimportant) {
                chunk.update_mesh_important = false;
                chunk.update_mesh_unimportant = false;
                game::update_mesh(chunk, building_quads);
                break;
            }
        }
    }
}