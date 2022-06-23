#include "face_mesh_generation.hpp"

using namespace game;

std::size_t game::get_block_vertex_count(const block& block) {
    return
        get_face_vertex_count<block::face::FRONT>(block) +
        get_face_vertex_count<block::face::BACK>(block) +
        get_face_vertex_count<block::face::TOP>(block) +
        get_face_vertex_count<block::face::BOTTOM>(block) +
        get_face_vertex_count<block::face::RIGHT>(block) +
        get_face_vertex_count<block::face::LEFT>(block) + 
        get_general_vertex_count(block);
}