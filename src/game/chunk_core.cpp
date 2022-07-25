#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "chunk_mesh_generation.hpp"
#include "common.hpp"
#include "glm/gtc/noise.hpp"
#include "block_functionality.hpp"
#include <algorithm>
using namespace game;

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