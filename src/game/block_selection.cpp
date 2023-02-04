#include "block_selection.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation.inl"
#include "block_functionality.hpp"
#include "block_mesh_generation.hpp"
#include "rendering.hpp"

using namespace game;

#define MATRIX_INDEX GX_PNMTX4

static std::optional<math::vector3s32> last_block_pos;
static std::optional<block> last_block;

static gfx::display_list disp_list;
static math::transform_3d tf;

static bool cull_back = true;

void block_selection_update(const Mtx view) {
    tf.update_model_view(view);
    tf.load(MATRIX_INDEX);
}

void block_selection_draw(chrono::us now) {
    GX_SetNumTevStages(1);
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);

    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    u8 alpha = 0x5f + (std::sin(now / 150000.0f) * 0x10);
    GX_SetTevColor(GX_TEVREG1, { 0xff, 0xff, 0xff, alpha });
    GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
    GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
    GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    //

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);

    GX_SetCurrentMtx(MATRIX_INDEX);

    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GX_SetZCompLoc(GX_TRUE);

    GX_SetCullMode(cull_back ? GX_CULL_BACK : GX_CULL_NONE);
    disp_list.call();

    if (!cull_back) {
        GX_SetCullMode(GX_CULL_BACK);
    }
}

struct block_selection_quad {
    std::array<math::vector3u8, 4> verts;
    
    template<typename T>
    inline block_selection_quad(const T& quad) : verts({ quad.vert0.pos, quad.vert1.pos, quad.vert2.pos, quad.vert3.pos }) { }
};

struct block_selection_mesh_state {
    block_selection_quad* back_cull_it;
    block_selection_quad* no_cull_it;

    template<typename L>
    inline void add_quad(const L::chunk_quad& quad) {
        *back_cull_it++ = quad;
    }
};

static void update_mesh(const Mtx view, decltype(chunk_quad_building_arrays::standard)& building_array, const block_raycast_t& raycast) {
    tf.set_position(view, raycast.location.ch_pos.x * chunk::size, raycast.location.ch_pos.y * chunk::size, raycast.location.ch_pos.z * chunk::size);
    tf.load(MATRIX_INDEX);

    auto begin = (block_selection_quad*)building_array.data();

    block_selection_mesh_state ms_st = {
        .back_cull_it = begin,
        .no_cull_it = begin
    };

    // TODO: actually use real block data
    block air_block = { .tp = block::type::air };

    const auto& block = *raycast.location.bl;
    math::vector3u8 block_pos = raycast.location.bl_pos;
    
    call_with_block_functionality(block.tp, [&ms_st, &air_block, &block, block_pos]<typename BF>() {
        add_block_vertices<BF>(ms_st, [&air_block]<block::face FACE>() { return &air_block; }, (game::block::state)game::block::slab_state::bottom, block_pos);
    });

    cull_back = true;
    auto end = ms_st.back_cull_it;
    std::size_t vert_count = 4 * (end - begin);
    if (vert_count == 0) {
        cull_back = false;
        end = ms_st.no_cull_it;
        vert_count = 4 * (end - begin);
    }

    disp_list.resize(
        gfx::get_begin_instruction_size(vert_count) +
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
        gfx::get_vector_instruction_size<4, u8>(vert_count) // Color
    );

    constexpr auto write_vertex = [](auto& vert) {
        GX_Position3u8(vert.x, vert.y, vert.z);
    };

    disp_list.write_into([begin, end, vert_count, write_vertex]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vert_count);
        for (auto it = begin; it != end; ++it) {
            write_vertex(it->verts[0]);
            write_vertex(it->verts[1]);
            write_vertex(it->verts[2]);
            write_vertex(it->verts[3]);
        }
        GX_End();
    });
}

void block_selection_handle_raycast(const Mtx view, decltype(chunk_quad_building_arrays::standard)& building_array, const block_raycast_wrap_t& raycast) {
    if (raycast.success) {
        // Check if we have a new selected block
        if (!last_block_pos.has_value() || raycast.val.location.bl_pos != last_block_pos || *raycast.val.location.bl != *last_block) {
            update_mesh(view, building_array, raycast.val);
        }

        last_block_pos = raycast.val.location.bl_pos;
        last_block = *raycast.val.location.bl;
    }
}