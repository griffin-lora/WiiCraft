// #include "water_overlay.hpp"

// using namespace game;

// water_overlay::water_overlay() {
//     tf.set_position(0, 0);
//     tf.load(mat);

//     constexpr std::size_t num_vertices = 4;
    
//     std::size_t disp_list_size = (
// 		gfx::get_begin_instruction_size(num_vertices) +
// 		gfx::get_vector_instruction_size<2, u16>(num_vertices) + // Position
// 		gfx::get_vector_instruction_size<2, u8>(num_vertices) // UV
// 	);

//     disp_list.resize(disp_list_size);

//     disp_list.write_into([num_vertices]() {
//         GX_Begin(GX_QUADS, GX_VTXFMT0, num_vertices);

//         GX_Position2u16(0, 0);
//         GX_TexCoord2u8(8, 0);
//         GX_Position2u16(640, 0);
//         GX_TexCoord2u8(9, 0);
//         GX_Position2u16(640, 480);
//         GX_TexCoord2u8(9, 1);
//         GX_Position2u16(0, 480);
//         GX_TexCoord2u8(8, 1);

//         GX_End();
//     });
// }

// void water_overlay::draw(const camera& cam) const {
//     // auto block = get_block_from_world_position(chunks, cam.position);
//     // if (block.has_value() && block->get().tp == block::type::water && does_world_position_select_block(cam.position, *block, floor_float_position<f32>(cam.position))) {
//     //     GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

//     //     GX_SetCurrentMtx(mat);

//     //     disp_list.call();
//     // }
// }

#include "water_overlay.h"

void water_overlay_init(void) {}
void water_overlay_draw(void) {}