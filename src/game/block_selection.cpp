#include "block_selection.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation.inl"

using namespace game;

static void init_drawing() {
	GX_SetNumChans(1);
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

void game::draw_block_selection(math::matrix view, const camera& cam, block_selection& bl_sel, std::optional<raycast>& raycast) {
    init_drawing();
    if (cam.update_view) {
        gfx::update_model_view(bl_sel.pos_state, view);
    }
    if (raycast.has_value()) {
        gfx::load(bl_sel.pos_state);
	    bl_sel.disp_list.call();
    }
}

struct block_selection_vert_func {
    static inline void call(u8 x, u8 y, u8 z, u8, u8) {
        GX_Position3u8(x, y, z);
        GX_Color4u8(0xff, 0xff, 0xff, 0x7f);
    }
};

static void update_mesh(math::matrix view, block_selection& bl_sel, const math::vector3s32& ch_pos, math::vector3u8 bl_pos, block::type type) {
    gfx::set_position(bl_sel.pos_state, view, ch_pos.x * game::chunk::SIZE, ch_pos.y * game::chunk::SIZE, ch_pos.z * game::chunk::SIZE);

    auto vertex_count = game::get_block_vertex_count(type);
    std::size_t disp_list_size = (
        4 + // GX_Begin
        vertex_count * 3 + // GX_Position3u8
        vertex_count * 4 + // GX_Color4u8
        1 // GX_End
    );

    bl_sel.disp_list.resize(disp_list_size);

    bl_sel.disp_list.write_into([&bl_pos, type, vertex_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);
        game::add_block_vertices<block_selection_vert_func>(bl_pos, type);
        GX_End();
    });
}

void game::handle_raycast(math::matrix view, block_selection& bl_sel, std::optional<raycast>& raycast) {
    if (raycast.has_value()) {
        // Check if we have a new selected block
        if (!bl_sel.last_block_pos.has_value() || raycast->bl_pos != bl_sel.last_block_pos) {
            update_mesh(view, bl_sel, raycast->ch_pos, raycast->bl_pos, raycast->bl.tp);
        }

        bl_sel.last_block_pos = raycast->bl_pos;
    }
}