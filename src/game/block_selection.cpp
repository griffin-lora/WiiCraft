#include "block_selection.hpp"
#include "face_mesh_generation.hpp"

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


void game::handle_raycast(math::matrix view, block_selection& bl_sel, std::optional<raycast>& raycast) {
    if (raycast.has_value()) {
        // Check if we have a new selected block
        if (!bl_sel.last_block_pos.has_value() || raycast->bl_pos != bl_sel.last_block_pos) {
            auto& pos = raycast->ch_pos;
            gfx::set_position(bl_sel.pos_state, view, pos.x * game::chunk::SIZE, pos.y * game::chunk::SIZE, pos.z * game::chunk::SIZE);

            auto vertex_count = game::get_block_vertex_count(raycast->bl.tp);
            std::size_t disp_list_size = (
                4 + // GX_Begin
                vertex_count * 3 + // GX_Position3u8
                vertex_count * 4 + // GX_Color4u8
                1 // GX_End
            );

            bl_sel.disp_list.resize(disp_list_size);

            bl_sel.disp_list.write_into([&raycast, vertex_count]() {
                GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);
                game::add_block_vertices<game::block_selection_vert_func>(raycast->bl_pos, raycast->bl.tp);
                GX_End();
            });
        }

        bl_sel.last_block_pos = raycast->bl_pos;
    }
}