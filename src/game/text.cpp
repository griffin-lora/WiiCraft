#include "text.hpp"

using namespace game;

void text::update(std::string_view str, u16 char_width, u16 char_height) {
    std::size_t vertex_count = str.size() * 4;

    std::size_t disp_list_size = (
		4 + // GX_Begin
		vertex_count * 4 + // GX_Position2u16
		vertex_count * 2 + // GX_TexCoord2u8
		1 // GX_End
	);

    disp_list.resize(disp_list_size);

    disp_list.write_into([&]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        math::vector2u16 pos = { 0, 0 };

        for (auto ch : str) {
            math::vector2u16 pos_offset = { pos.x + char_width, pos.y + char_height };

            auto uv = get_2d_position_from_index(ch);
            math::vector2u8 uv_offset = { uv.x + 1, uv.y + 1 };

            GX_Position2u16(pos.x, pos.y);
            GX_TexCoord2u8(uv.x, uv.y);
            GX_Position2u16(pos_offset.x, pos.y);
            GX_TexCoord2u8(uv_offset.x, uv.y);
            GX_Position2u16(pos_offset.x, pos_offset.y);
            GX_TexCoord2u8(uv_offset.x, uv_offset.y);
            GX_Position2u16(pos.x, pos_offset.y);
            GX_TexCoord2u8(uv.x, uv_offset.y);

            pos.x += char_width;
        }

        GX_End();
    });
}

void text::draw() {
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP3, GX_COLOR0A0);

    tf.load(GX_PNMTX3);

    disp_list.call();
}