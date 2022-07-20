#include "block_selection.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"
#include "block_mesh_generation.inl"
#include "rendering.hpp"

using namespace game;

void block_selection::update_if_needed(const math::matrix view, const camera& cam) {
    if (cam.update_view) {
        tf.update_model_view(view);
        tf.load(MAT);
    }
}

void block_selection::draw(const std::optional<block_raycast>& raycast) const {
    if (raycast.has_value()) {
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

        GX_SetCurrentMtx(MAT);

        init_standard_rendering();
	    standard_disp_list.call();

        init_foliage_rendering();
	    foliage_disp_list.call();

        init_water_rendering();
	    water_disp_list.call();
    }
}

void block_selection::update_mesh(const math::matrix view, block_quad_building_arrays& building_arrays, const block_raycast& raycast) {
    tf.set_position(view, raycast.location.ch_pos.x * chunk::SIZE, raycast.location.ch_pos.y * chunk::SIZE, raycast.location.ch_pos.z * chunk::SIZE);
    tf.load(MAT);

    block_mesh_state ms_st = {
        .it = { building_arrays }
    };

    add_block_vertices(ms_st, raycast.location.bl_pos, *raycast.location.bl);

    write_into_display_lists({ building_arrays }, ms_st.it, standard_disp_list, foliage_disp_list, water_disp_list, [](auto vert_count) {
        return (
            (vert_count > 0xff ? 4 : 3) + // GX_Begin
            vert_count * 3 + // GX_Position3u8
            vert_count * 4 // GX_Color4u8
        );
    }, [](auto& vert) {
        GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
        GX_Color4u8(0xff, 0xff, 0xff, 0x7f);
    });
}

void block_selection::handle_raycast(const math::matrix view, block_quad_building_arrays& building_arrays, const std::optional<block_raycast>& raycast) {
    if (raycast.has_value()) {
        // Check if we have a new selected block
        if (!last_block_pos.has_value() || raycast->location.bl_pos != last_block_pos || *raycast->location.bl != *last_block) {
            update_mesh(view, building_arrays, *raycast);
        }

        last_block_pos = raycast->location.bl_pos;
        last_block = *raycast->location.bl;
    }
}