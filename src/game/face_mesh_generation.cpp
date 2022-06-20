#include "face_mesh_generation.hpp"

using namespace game;

std::size_t game::get_block_vertex_count(block::type type) {
    return
        get_face_vertex_count<block::face::FRONT>(type) +
        get_face_vertex_count<block::face::BACK>(type) +
        get_face_vertex_count<block::face::TOP>(type) +
        get_face_vertex_count<block::face::BOTTOM>(type) +
        get_face_vertex_count<block::face::RIGHT>(type) +
        get_face_vertex_count<block::face::LEFT>(type);
}