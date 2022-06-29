#include "block_selection.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"

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

    // TODO: Properly seperate the standard and foliage meshes to avoid this
	GX_SetCullMode(GX_CULL_NONE);
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

void block_selection::update_mesh(const math::matrix view, ext::data_array<chunk::quad>& building_quads, const raycast& raycast) {
    // tf.set_position(view, raycast.ch_pos.x * chunk::SIZE, raycast.ch_pos.y * chunk::SIZE, raycast.ch_pos.z * chunk::SIZE);

    // struct {
    //     ext::data_array<chunk::quad>::iterator quad_it;

    //     inline void add(u8 x, u8 y, u8 z, u8, u8) {
    //         *vert_it++ = {
    //             .pos = { x, y, z }
    //         };
    //     }

    //     inline void add_foliage(u8 x, u8 y, u8 z, u8, u8) {
    //         *vert_it++ = {
    //             .pos = { x, y, z }
    //         };
    //     }
    // } vf = {
    //     .vert_it = building_verts.begin()
    // };

    // add_block_vertices(vf, raycast.bl_pos, raycast.bl);

    // auto vertex_count = vf.vert_it - building_verts.begin();
    // std::size_t disp_list_size = (
    //     4 + // GX_Begin
    //     vertex_count * 3 + // GX_Position3u8
    //     vertex_count * 4 + // GX_Color4u8
    //     1 // GX_End
    // );

    // disp_list.resize(disp_list_size);

    // disp_list.write_into([&building_verts, &vf, vertex_count]() {
    //     GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

    //     for (auto it = building_verts.begin(); it != vf.vert_it; ++it) {
    //         GX_Position3u8(it->pos.x, it->pos.y, it->pos.z);
    //         GX_Color4u8(0xff, 0xff, 0xff, 0x7f);
    //     }
        
    //     GX_End();
    // });
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