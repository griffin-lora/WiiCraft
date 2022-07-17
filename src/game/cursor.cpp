#include "cursor.hpp"

using namespace game;

cursor::cursor() {
    constexpr std::size_t vertex_count = 4;
    
    std::size_t disp_list_size = (
		3 + // GX_Begin
		vertex_count * 2 + // GX_Position2u8
		vertex_count * 2 // GX_TexCoord2u8
	);

    disp_list.resize(disp_list_size);

    disp_list.write_into([vertex_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        GX_Position2u16(0, 0);
        GX_TexCoord2u8(0, 0);
        GX_Position2u16(48, 0);
        GX_TexCoord2u8(1, 0);
        GX_Position2u16(48, 48);
        GX_TexCoord2u8(1, 1);
        GX_Position2u16(0, 48);
        GX_TexCoord2u8(0, 1);

        GX_End();
    });
}

void cursor::update_from_pointer_position(u16 v_width, u16 v_height, const std::optional<glm::vec2>& pointer_pos) {
    if (pointer_pos.has_value()) {
        tf.set_position(pointer_pos->x - 24.f, pointer_pos->y - 24.f);
        tf.load(MAT);
    } else {
        tf.set_position((v_width / 2) - 24.f, (v_height / 2) - 24.f);
        tf.load(MAT);
    }
}

void cursor::draw() const {
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
    
	GX_SetCurrentMtx(MAT);

    disp_list.call();
}