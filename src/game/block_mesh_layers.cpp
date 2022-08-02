#include "block_mesh_layers.hpp"
#include "gfx/display_list.hpp"

using namespace game;

std::size_t standard_block_mesh_layer::get_chunk_display_list_size(std::size_t vert_count) {
    return
        gfx::get_begin_instruction_size(vert_count) +
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
        gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
}

void standard_block_mesh_layer::write_chunk_vertex(const chunk_quad::vertex& vert) {
    GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
    GX_TexCoord2u8(vert.uv.x, vert.uv.y);
}

static std::size_t get_tinted_chunk_display_list_size(std::size_t vert_count) {
    return
        gfx::get_begin_instruction_size(vert_count) +
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Color
        gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
}

static void write_tinted_chunk_vertex(const auto& vert) {
    GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
    GX_Color3u8(vert.color.r, vert.color.g, vert.color.b);
    GX_TexCoord2u8(vert.uv.x, vert.uv.y);
}

std::size_t tinted_block_mesh_layer::get_chunk_display_list_size(std::size_t vert_count) {
    return get_tinted_chunk_display_list_size(vert_count);
}

void tinted_block_mesh_layer::write_chunk_vertex(const chunk_quad::vertex& vert) {
    write_tinted_chunk_vertex(vert);
}

std::size_t tinted_decal_block_mesh_layer::get_chunk_display_list_size(std::size_t vert_count) {
    return
        gfx::get_begin_instruction_size(vert_count) +
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Color
        gfx::get_vector_instruction_size<2, u8>(vert_count) + // UV
        gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
}

void tinted_decal_block_mesh_layer::write_chunk_vertex(const chunk_quad::vertex& vert) {
    GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
    GX_Color3u8(vert.color.r, vert.color.g, vert.color.b);
    GX_TexCoord2u8(vert.uvs[0].x, vert.uvs[0].y);
    GX_TexCoord2u8(vert.uvs[1].x, vert.uvs[1].y);
}

std::size_t tinted_double_side_alpha_block_mesh_layer::get_chunk_display_list_size(std::size_t vert_count) {
    return get_tinted_chunk_display_list_size(vert_count);
}

void tinted_double_side_alpha_block_mesh_layer::write_chunk_vertex(const chunk_quad::vertex& vert) {
    write_tinted_chunk_vertex(vert);
}