#include "block_selection.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"
#include "mesh_generation.hpp"
#include "mesh_generation.inl"

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

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
}

void block_selection::update_if_needed(const math::matrix view, const camera& cam) {
    if (cam.update_view) {
        tf.update_model_view(view);
    }
}

void block_selection::draw_first(const std::optional<raycast>& raycast) const {
    if (raycast.has_value()) {
        init_drawing();
        tf.load(GX_PNMTX3);

        GX_SetCullMode(GX_CULL_BACK);
	    standard_disp_list.call();
    }
}

void block_selection::draw_second(const std::optional<raycast>& raycast) const {
    if (raycast.has_value()) {
        init_drawing();
        tf.load(GX_PNMTX3);

        GX_SetCullMode(GX_CULL_NONE);
	    foliage_disp_list.call();
    }
}

void block_selection::update_mesh(const math::matrix view, ext::data_array<chunk::quad>& building_quads, const raycast& raycast) {
    tf.set_position(view, raycast.ch_pos.x * chunk::SIZE, raycast.ch_pos.y * chunk::SIZE, raycast.ch_pos.z * chunk::SIZE);

    standard_vertex_function vf = {
        .quad_it = building_quads.begin()
    };

    add_block_vertices(vf, raycast.bl_pos, raycast.bl);

    write_into_display_lists(building_quads, vf, standard_disp_list, foliage_disp_list, [](auto vert_count) {
        return (
            4 + // GX_Begin
            vert_count * 3 + // GX_Position3u8
            vert_count * 4 + // GX_Color4u8
            1 // GX_End
        );
    }, [](auto& vert) {
        GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
        GX_Color4u8(0xff, 0xff, 0xff, 0x7f);
    });
}

void block_selection::handle_raycast(const math::matrix view, ext::data_array<chunk::quad>& building_quads, const std::optional<raycast>& raycast) {
    if (raycast.has_value()) {
        // Check if we have a new selected block
        if (!last_block_pos.has_value() || raycast->bl_pos != last_block_pos) {
            update_mesh(view, building_quads, *raycast);
        }

        last_block_pos = raycast->bl_pos;
    }
}