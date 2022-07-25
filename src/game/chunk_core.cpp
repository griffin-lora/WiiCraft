#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "chunk_mesh_generation.hpp"
#include "common.hpp"
#include "glm/gtc/noise.hpp"
#include "block_functionality.hpp"
#include <algorithm>

using namespace game;
using math::get_noise_at;

static f32 get_hills_height(glm::vec2 position) {
    position /= 32.0f;

    return 2.0f * (
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

static void generate_high_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    block block = { .tp = block::type::AIR };
    std::fill(chunk.blocks.begin(), chunk.blocks.end(), block);
    get_block_count_ref(chunk, block) = chunk.blocks.size();
}

static constexpr s32 Z_OFFSET = chunk::SIZE * chunk::SIZE;
static constexpr s32 Y_OFFSET = chunk::SIZE;
static constexpr s32 X_OFFSET = 1;

static void generate_middle_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    auto it = chunk.blocks.begin();

    f32 world_chunk_x = chunk_pos.x * chunk::SIZE;
    f32 world_chunk_z = chunk_pos.z * chunk::SIZE;

    for (f32 z = 0; z < chunk::SIZE; z++) {
        for (f32 x = 0; x < chunk::SIZE; x++) {
            f32 world_x = world_chunk_x + x;
            f32 world_z = world_chunk_z + z;
            glm::vec2 noise_pos = { world_x, world_z };

            f32 height = get_hills_height(noise_pos);
            
            f32 tallgrass_value = get_tallgrass_value(noise_pos);

            s32 gen_y = (height * 12) + 1;

            for (s32 y = 0; y < chunk::SIZE; y++) {
                auto& block = *it;

                if (y > gen_y) {
                    if (y < 7) {
                        block = { .tp = block::type::WATER };
                    } else if (y == (gen_y + 1) && gen_y >= 7 && tallgrass_value > 0.97f) {
                        block = { .tp = block::type::TALL_GRASS };
                    } else {
                        block = { .tp = block::type::AIR };
                    }
                } else {
                    if (gen_y < 7) {
                        if (y < (gen_y - 2)) {
                            block = { .tp = block::type::STONE };
                        } else {
                            block = { .tp = block::type::SAND };
                        }
                    } else if (y < gen_y) {
                        if (y < (gen_y - 2)) {
                            block = { .tp = block::type::STONE };
                        } else {
                            block = { .tp = block::type::DIRT };
                        }
                    } else {
                        block = { .tp = block::type::GRASS };
                    }
                }

                get_block_count_ref(chunk, block)++;

                it += Y_OFFSET;
            }

            it += X_OFFSET - Z_OFFSET;
        }
        it += Z_OFFSET - Y_OFFSET;
    }
}

static void generate_low_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    block block = { .tp = block::type::STONE };
    std::fill(chunk.blocks.begin(), chunk.blocks.end(), block);
    get_block_count_ref(chunk, block) = chunk.blocks.size();
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

std::size_t& game::get_block_count_ref(chunk& chunk, const block& block) {
    auto counting_type = get_with_block_functionality<block_counting_type>(block.tp, [&block]<typename Bf>() {
        return Bf::get_block_counting_type(block.st);
    });
    switch (counting_type) {
        case block_counting_type::INVISIBLE: return chunk.invisible_block_count;
        case block_counting_type::FULLY_OPAQUE: return chunk.fully_opaque_block_count;
        case block_counting_type::PARTIALLY_OPAQUE: return chunk.partially_opaque_block_count;
    }
    // Compiler complains despite this being unreachable
    return chunk.invisible_block_count;
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

void game::update_chunks(block_quad_building_arrays& building_arrays, chunk::map& chunks, chrono::us& total_mesh_gen_time, chrono::us& last_mesh_gen_time, chrono::us now) {
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
                if (!chunk.update_shell_mesh_unimportant) {
                    break;
                }
            }
            if (chunk.update_shell_mesh_unimportant) {
                chunk.update_shell_mesh_important = false;
                chunk.update_shell_mesh_unimportant = false;
                auto start = chrono::get_current_us();
                update_shell_mesh(building_arrays, chunk);
                total_mesh_gen_time += chrono::get_current_us() - start;
                // Don't track MGL here as well

                if (chunk.fade_in_when_mesh_is_updated) {
                    chunk.fade_in_when_mesh_is_updated = false;
                    chunk.fade_in = true;
                    chunk.fade_in_start = now;
                }

                break;
            }
        }
    }

    for (auto& [ pos, chunk ] : chunks) {
        if (chunk.fade_in) {
            auto elapsed = now - chunk.fade_in_start;
            if (elapsed <= chunk::FADE_IN_TIME) {
                auto lerp_alpha = math::get_eased(elapsed / (f32)chunk::FADE_IN_TIME);

                chunk.alpha = math::lerp(0x0, 0xff, lerp_alpha);
            } else {
                chunk.fade_in = false;
            }
        }
    }
}