#include "water_overlay.hpp"
#include "chunk_core.hpp"

using namespace game;

water_overlay::water_overlay() {
    tf.set_position(0, 0);
    tf.load(MAT);

    constexpr std::size_t vertex_count = 4;
    
    std::size_t disp_list_size = (
		gfx::get_begin_instruction_size(vertex_count) +
		gfx::get_vector_instruction_size<2, u16>(vertex_count) + // Position
		gfx::get_vector_instruction_size<2, u8>(vertex_count) // UV
	);

    disp_list.resize(disp_list_size);

    disp_list.write_into([vertex_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        GX_Position2u16(0, 0);
        GX_TexCoord2u8(8, 0);
        GX_Position2u16(640, 0);
        GX_TexCoord2u8(9, 0);
        GX_Position2u16(640, 480);
        GX_TexCoord2u8(9, 1);
        GX_Position2u16(0, 480);
        GX_TexCoord2u8(8, 1);

        GX_End();
    });
}

void water_overlay::draw(const camera& cam, const chunk::map& chunks) const {
    auto block = get_block_from_world_position(chunks, cam.position);
    if (block.has_value() && block->get().tp == block::type::WATER && does_world_position_select_block(cam.position, *block, floor_float_position<f32>(cam.position))) {
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

        GX_SetCurrentMtx(MAT);

        disp_list.call();
    }
}