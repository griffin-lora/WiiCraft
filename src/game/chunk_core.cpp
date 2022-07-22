#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "chunk_mesh_generation.hpp"
#include "common.hpp"
#include "glm/gtc/noise.hpp"
#include <algorithm>

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

static f32 get_hills_height(glm::vec2 position) {
    position /= 64.0f;

    return (
        (get_noise_at(position) * 0.9f) +
        (get_noise_at(position * 4.0f) * 0.6f) +
        (get_noise_at(position * 7.0f) * 0.3f)
    );
}

static f32 get_tallgrass_value(glm::vec2 position) {
    position /= 2.0f;

    return (
        (get_noise_at(position) * 0.5f) + 
        (get_noise_at(position * 2.0f) * 1.0f)
    );
}

static void generate_high_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    std::fill(chunk.blocks.begin(), chunk.blocks.end(), block{ .tp = block::type::AIR });
    get_block_count_ref(chunk, block::type::AIR) = chunk.blocks.size();
}

static constexpr s32 Z_OFFSET = chunk::SIZE * chunk::SIZE;
static constexpr s32 Y_OFFSET = chunk::SIZE;
static constexpr s32 X_OFFSET = 1;

static void generate_middle_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    auto it = chunk.blocks.begin();

    f32 world_chunk_x = chunk_pos.x * chunk::SIZE;
    s32 world_chunk_y = chunk_pos.y * chunk::SIZE;
    f32 world_chunk_z = chunk_pos.z * chunk::SIZE;

    for (f32 z = 0; z < chunk::SIZE; z++) {
        for (f32 x = 0; x < chunk::SIZE; x++) {
            f32 world_x = world_chunk_x + x;
            f32 world_z = world_chunk_z + z;
            glm::vec2 noise_pos = { world_x, world_z };

            f32 plains_height = get_hills_height(noise_pos);
            
            f32 tallgrass_value = get_tallgrass_value(noise_pos);

            s32 generated_height = (plains_height * 12) + 1;

            for (s32 y = 0; y < chunk::SIZE; y++) {
                s32 world_height = world_chunk_y + y;

                auto& block = *it;

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

                get_block_count_ref(chunk, block.tp)++;

                it += Y_OFFSET;
            }

            it += X_OFFSET - Z_OFFSET;
        }
        it += Z_OFFSET - Y_OFFSET;
    }
}

static void generate_low_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    std::fill(chunk.blocks.begin(), chunk.blocks.end(), block{ .tp = block::type::STONE });
    get_block_count_ref(chunk, block::type::STONE) = chunk.blocks.size();
}

void game::generate_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    if (chunk_pos.y > 0) {
        generate_high_blocks(chunk, chunk_pos);
    } else if (chunk_pos.y < 0) {
        generate_low_blocks(chunk, chunk_pos);
    } else {
        generate_middle_blocks(chunk, chunk_pos);
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

void game::add_important_chunk_mesh_update(chunk& chunk, const math::vector3s32& pos) {
    chunk.update_core_mesh_important = true;
    chunk.update_shell_mesh_important = true;
    call_func_on_each_face<void>([&chunk, &pos]<block::face face>() {
        if (is_block_position_at_face_edge<face>(pos)) {
            auto nb_chunk = get_neighbor<face>(chunk.nh);
            if (nb_chunk.has_value()) {
                nb_chunk->get().update_shell_mesh_important = true;
            }
        }
    });
}

std::size_t& game::get_block_count_ref(chunk& chunk, block::type tp) {
    // TODO: Use block functionality
    switch (tp) {
        default:
        case block::type::AIR:
            return chunk.invisible_block_count;
        case block::type::DEBUG:
        case block::type::GRASS:
        case block::type::STONE:
        case block::type::DIRT:
        case block::type::WOOD_PLANKS:
            return chunk.fully_transparent_block_count;
        case block::type::STONE_SLAB:
        case block::type::TALL_GRASS:
        case block::type::WATER:
            return chunk.partially_transparent_block_count;
    }
}

void game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk) {
    call_func_on_each_face<void>([&chunk]<block::face face>() {
        auto nb_chunk = get_neighbor<face>(chunk.nh);
        if (nb_chunk.has_value()) {
            nb_chunk->get().update_shell_mesh_unimportant = true;
            nb_chunk->get().update_neighborhood = true;
        }
    });
}

void game::add_chunk_neighborhood_update_to_neighbors(chunk& chunk) {
    call_func_on_each_face<void>([&chunk]<block::face face>() {
        auto nb_chunk = get_neighbor<face>(chunk.nh);
        if (nb_chunk.has_value()) {
            nb_chunk->get().update_neighborhood = true;
        }
    });
}

void game::update_chunks(block_quad_building_arrays& building_arrays, chunk::map& chunks, chrono::us& total_mesh_gen_time, chrono::us& last_mesh_gen_time) {
    for (auto& [ pos, chunk ] : chunks) {
        if (chunk.update_neighborhood) {
            chunk.update_neighborhood = false;
            update_chunk_neighborhood(chunks, pos, chunk);
        }
    }

    bool did_important_mesh_update = false;
    for (auto& [ pos, chunk ] : chunks) {
        if (chunk.update_core_mesh_important) {
            did_important_mesh_update = true;
            chunk.update_core_mesh_important = false;
            chunk.update_core_mesh_unimportant = false;
            auto start = chrono::get_current_us();
            update_core_mesh(building_arrays, chunk);
            total_mesh_gen_time += chrono::get_current_us() - start;
            last_mesh_gen_time = chrono::get_current_us() - start;
        }

        if (chunk.update_shell_mesh_important) {
            did_important_mesh_update = true;
            chunk.update_shell_mesh_important = false;
            chunk.update_shell_mesh_unimportant = false;
            auto start = chrono::get_current_us();
            update_shell_mesh(building_arrays, chunk);
            total_mesh_gen_time += chrono::get_current_us() - start;
            // Don't track MGL since its so small for this
        }
    }

    if (!did_important_mesh_update) {
        for (auto& [ pos, chunk ] : chunks) {
            if (chunk.update_core_mesh_unimportant) {
                chunk.update_core_mesh_important = false;
                chunk.update_core_mesh_unimportant = false;
                auto start = chrono::get_current_us();
                update_core_mesh(building_arrays, chunk);
                total_mesh_gen_time += chrono::get_current_us() - start;
                last_mesh_gen_time = chrono::get_current_us() - start;
                break;
            } else if (chunk.update_shell_mesh_unimportant) {
                chunk.update_shell_mesh_important = false;
                chunk.update_shell_mesh_unimportant = false;
                auto start = chrono::get_current_us();
                update_shell_mesh(building_arrays, chunk);
                total_mesh_gen_time += chrono::get_current_us() - start;
                // Don't track MGL here as well
                break;
            }
        }
    }
}