#include "block_selection.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"

using namespace game;

static void init_drawing() {
	GX_SetNumChans(1);

    GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
	GX_SetChanAmbColor(GX_COLOR0A0, { 0xff, 0xff, 0xff, 0x7f });
	GX_SetChanMatColor(GX_COLOR0A0, { 0xff, 0xff, 0xff, 0x7f });

	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	//

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
}

void block_selection::update_if_needed(const math::matrix view, const camera& cam) {
    if (cam.update_view) {
        tf.update_model_view(view);
    }
}

void block_selection::draw(const std::optional<raycast>& raycast) const {
    if (raycast.has_value()) {
        init_drawing();
        tf.load(GX_PNMTX3);
	    disp_list.call();
    }
}

void block_selection::update_mesh(const math::matrix view, const math::vector3s32& ch_pos, math::vector3u8 bl_pos, block::type type) {
    tf.set_position(view, ch_pos.x * game::chunk::SIZE, ch_pos.y * game::chunk::SIZE, ch_pos.z * game::chunk::SIZE);

    auto vertex_count = game::get_block_vertex_count(type);
    std::size_t disp_list_size = (
        4 + // GX_Begin
        vertex_count * 3 + // GX_Position3u8
        vertex_count * 4 + // GX_Color4u8
        1 // GX_End
    );

    disp_list.resize(disp_list_size);

    disp_list.write_into([&bl_pos, type, vertex_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);
        auto vf = [](u8 x, u8 y, u8 z, s8, s8, s8, u8, u8) {
            GX_Position3u8(x, y, z);
            GX_Color4u8(0xff, 0xff, 0xff, 0x7f);
        };
        game::add_block_vertices(vf, bl_pos, type);
        GX_End();
    });
}

void block_selection::handle_raycast(const math::matrix view, const std::optional<raycast>& raycast) {
    if (raycast.has_value()) {
        // Check if we have a new selected block
        if (!last_block_pos.has_value() || raycast->bl_pos != last_block_pos) {
            update_mesh(view, raycast->ch_pos, raycast->bl_pos, raycast->bl.tp);
        }

        last_block_pos = raycast->bl_pos;
    }
}