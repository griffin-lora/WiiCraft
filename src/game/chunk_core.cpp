#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "chunk_mesh_generation.hpp"
#include "common.hpp"
#include "glm/gtc/noise.hpp"
#include "block_functionality.hpp"
#include <algorithm>
using namespace game;

template<block::face FACE>
static chunk::opt_ref get_neighbor_from_map(chunk::map& chunks, const math::vector3s32& pos) {
    math::vector3s32 offset_pos = get_face_offset_position<FACE>(pos);
    auto it = chunks.find(offset_pos);
    if (it != chunks.end()) {
        return it->second;
    } else {
        return {};
    }
}

void game::update_chunk_neighborhood(chunk::map& chunks, const math::vector3s32& pos, chunk& chunk) {
    chunk.nh = {
        .front = get_neighbor_from_map<block::face::front>(chunks, pos),
        .back = get_neighbor_from_map<block::face::back>(chunks, pos),
        .top = get_neighbor_from_map<block::face::top>(chunks, pos),
        .bottom = get_neighbor_from_map<block::face::bottom>(chunks, pos),
        .right = get_neighbor_from_map<block::face::right>(chunks, pos),
        .left = get_neighbor_from_map<block::face::left>(chunks, pos),
    };
}

void game::add_important_chunk_mesh_update(chunk& chunk, const math::vector3s32& pos) {
    chunk.update_core_mesh_important = true;
    chunk.update_shell_mesh_important = true;
    call_func_on_each_face<void>([&chunk, &pos]<block::face FACE>() {
        if (is_block_position_at_face_edge<FACE>(pos)) {
            auto nb_chunk = get_neighbor<FACE>(chunk.nh);
            if (nb_chunk.has_value()) {
                nb_chunk->get().update_shell_mesh_important = true;
            }
        }
    });
}

std::size_t& game::get_block_count_ref(chunk& chunk, const block& block) {
    auto counting_type = get_with_block_functionality<block_counting_type>(block.tp, [&block]<typename BF>() {
        return BF::get_block_counting_type((game::block::state)game::block::slab_state::bottom);
    });
    switch (counting_type) {
        case block_counting_type::invisible: return chunk.invisible_block_count;
        case block_counting_type::fully_opaque: return chunk.fully_opaque_block_count;
        case block_counting_type::partially_opaque: return chunk.partially_opaque_block_count;
    }
    // Compiler complains despite this being unreachable
    return chunk.invisible_block_count;
}

void game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk) {
    call_func_on_each_face<void>([&chunk]<block::face FACE>() {
        auto nb_chunk = get_neighbor<FACE>(chunk.nh);
        if (nb_chunk.has_value()) {
            nb_chunk->get().update_shell_mesh_unimportant = true;
            nb_chunk->get().update_neighborhood = true;
        }
    });
}

void game::add_chunk_neighborhood_update_to_neighbors(chunk& chunk) {
    call_func_on_each_face<void>([&chunk]<block::face FACE>() {
        auto nb_chunk = get_neighbor<FACE>(chunk.nh);
        if (nb_chunk.has_value()) {
            nb_chunk->get().update_neighborhood = true;
        }
    });
}