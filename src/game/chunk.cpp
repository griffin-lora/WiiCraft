#include "game.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

math::vector3u8 game::get_position_from_index(std::size_t index) {
    uint z = index / (chunk::SIZE * chunk::SIZE);
    index -= (z * chunk::SIZE * chunk::SIZE);
    uint y = index / chunk::SIZE * chunk::SIZE;
    uint x = index % chunk::SIZE * chunk::SIZE;
    return { x, y, z };
}

void game::init(chunk& chunk, math::matrix view) {
    guMtxIdentity(chunk.model);
    guMtxTransApply(chunk.model, chunk.model, chunk.position.x * chunk::SIZE, chunk.position.y * chunk::SIZE, chunk.position.z * chunk::SIZE);
    update_model_view(chunk, view);
}

template<typename F>
inline void iter_chunk_vec(F func) {
    for (u8 x = 0; x < chunk::SIZE; ++x) {
        for (u8 y = 0; y < chunk::SIZE; ++y) {
            for (u8 z = 0; z < chunk::SIZE; ++z) {
                func({x, y, z});
            }
        }
    }
}

void game::generate_blocks(chunk& chunk) {
    iter_chunk_vec([&chunk](math::vector3u8 pos) {
        auto& block = chunk.blocks[get_index_from_position(pos)];
        if (pos.y == 2) {
            block = { .tp = block::type::DEBUG };
        } else {
            block = { .tp = block::type::AIR };
        }
    });
}

template<block::face face>
constexpr math::vector3u8 get_vector_moved_towards_face(math::vector3u8 inp) {
    math::vector3u8 vec = inp;
    if constexpr (face == block::face::FRONT) {
        vec.x += 1;
    } else if constexpr (face == block::face::BACK) {
        vec.x -= 1;
    } else if constexpr (face == block::face::LEFT) {
        vec.z += 1;
    } else if constexpr (face == block::face::RIGHT) {
        vec.z -= 1;
    } else if constexpr (face == block::face::TOP) {
        vec.y += 1;
    } else if constexpr (face == block::face::BOTTOM) {
        vec.y -= 1;
    }
    return vec;
}

bool is_block_visible(block block) {
    switch (block.tp) {
        default:
        case block::type::AIR:
            return false;
        case block::type::GRASS:
        case block::type::DEBUG:
            return true;
    }
}

template<block::face face>
bool should_render_face(const chunk& chunk, math::vector3u8 pos) {
    auto& block = chunk.blocks[get_index_from_position(pos)];
    if (is_block_visible(block)) {
        auto check_pos = get_vector_moved_towards_face<block::face::FRONT>(pos);
        if (check_pos.x >= chunk::SIZE || check_pos.y >= chunk::SIZE || check_pos.z >= chunk::SIZE) {
            return true;
        }
        auto& check_block = chunk.blocks[get_index_from_position(check_pos)];
        return !is_block_visible(check_block);
    } else {
        return false;
    }
}

template<block::face face>
std::size_t get_face_vertex_count_for_block(const chunk& chunk, math::vector3u8 pos) {
    if (should_render_face<face>(chunk, pos)) {
        return get_any_face_vertex_count(chunk.blocks[get_index_from_position(pos)].tp);
    } else {
        return 0;
    }
}

std::size_t get_vertex_count(const chunk& chunk) {
    std::size_t vertex_count = 0;
    iter_chunk_vec([&chunk, &vertex_count](math::vector3u8 pos) {
        vertex_count += get_face_vertex_count_for_block<block::face::FRONT>(chunk, pos);
        vertex_count += get_face_vertex_count_for_block<block::face::BACK>(chunk, pos);
        vertex_count += get_face_vertex_count_for_block<block::face::LEFT>(chunk, pos);
        vertex_count += get_face_vertex_count_for_block<block::face::RIGHT>(chunk, pos);
        vertex_count += get_face_vertex_count_for_block<block::face::TOP>(chunk, pos);
        vertex_count += get_face_vertex_count_for_block<block::face::BOTTOM>(chunk, pos);
    });
    return vertex_count;
}

template<block::face face>
void add_face_vertices_for_block(chunk& chunk, vertex_it& it, math::vector3u8 pos) {
    if (should_render_face<face>(chunk, pos)) {
        add_face_vertices_at_mut_it<face>(pos, it, chunk.blocks[get_index_from_position(pos)].tp);
    }
}

void game::update_mesh(chunk& chunk) {
    chunk.ms.vertices.resize_without_copying(get_vertex_count(chunk));

    auto it = chunk.ms.vertices.begin();

    iter_chunk_vec([&chunk, &it](math::vector3u8 pos) {
        add_face_vertices_for_block<block::face::FRONT>(chunk, it, pos);
        add_face_vertices_for_block<block::face::BACK>(chunk, it, pos);
        add_face_vertices_for_block<block::face::LEFT>(chunk, it, pos);
        add_face_vertices_for_block<block::face::RIGHT>(chunk, it, pos);
        add_face_vertices_for_block<block::face::TOP>(chunk, it, pos);
        add_face_vertices_for_block<block::face::BOTTOM>(chunk, it, pos);
    });
    if (it != chunk.ms.vertices.end()) {
        dbg::error([]() {
            std::printf("vertex count mismatch\n");
        });
    }
}

void game::draw_chunk_mesh_vertices(const ext::data_array<chunk::mesh::vertex>& vertices) {
    gfx::draw_quads(vertices.size(), [&vertices]() {
        for (auto& v : vertices) {
            gfx::draw_vertex((f32)v.local_position.x, (f32)v.local_position.y, (f32)v.local_position.z, ((f32)v.uv_position.x)/16.f, ((f32)v.uv_position.y)/16.f);
        }
    });
}

void game::draw_chunk(chunk& chunk) {
    // load the modelview matrix into matrix memory
    gfx::set_position_matrix_into_index(chunk.model_view, GX_PNMTX3);
    
    gfx::set_position_matrix_from_index(GX_PNMTX3);

    game::draw_chunk_mesh_vertices(chunk.ms.vertices);
}