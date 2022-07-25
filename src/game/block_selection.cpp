#include "block_selection.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation.inl"
#include "block_mesh_generation.inl"
#include "block_functionality.hpp"
#include "rendering.hpp"

using namespace game;

void block_selection::update_if_needed(const math::matrix view, const camera& cam) {
    if (cam.update_view) {
        tf.update_model_view(view);
        tf.load(MAT);
    }
}

void block_selection::draw(chrono::us now, const std::optional<block_raycast>& raycast) const {
    if (raycast.has_value()) {
        GX_SetNumTevStages(1);
        GX_SetNumChans(1);
        GX_SetNumTexGens(0);

        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

        u8 alpha = 0x5f + (std::sin(now / 150000.0f) * 0x10);
        GX_SetTevColor(GX_TEVREG1, { 0xff, 0xff, 0xff, alpha });
        GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
	    GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
	    GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

        //

        GX_ClearVtxDesc();
        GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);

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

    // TODO: actually use real block data
    block air_block = { .tp = block::type::AIR };

    const auto& block = *raycast.location.bl;
    math::vector3u8 block_pos = raycast.location.bl_pos;
    
    call_with_block_functionality(block.tp, [&ms_st, &air_block, &block, block_pos]<typename Bf>() {
        add_block_vertices<Bf>(ms_st, [&air_block]<block::face face>() { return &air_block; }, block.st, block_pos);
    });

    write_into_display_lists({ building_arrays }, ms_st.it, standard_disp_list, foliage_disp_list, water_disp_list, [](auto vert_count) {
        return (
            gfx::get_begin_instruction_size(vert_count) +
            gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
            gfx::get_vector_instruction_size<4, u8>(vert_count) // Color
        );
    }, [](auto& vert) {
        GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
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