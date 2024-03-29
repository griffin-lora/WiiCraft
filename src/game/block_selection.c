#include "block_selection.h"
#include "gfx/instruction_size.h"
#include "util.h"
#include "block.h"
#include <ogc/gx.h>
#include <ogc/cache.h>
#include <string.h>
#include <stdbool.h>
#include <stdalign.h>

#define MATRIX_INDEX GX_PNMTX4
#define VERTEX_FORMAT_INDEX GX_VTXFMT4

static bool has_last_block = false;
static u8vec3s last_block_pos;
static block_type_t last_block_type;

static Mtx model;
static Mtx model_view;

static size_t disp_list_size = 0;
static bool cull_back = true;

#define NUM_CUBE_VERTICES 24

#define CUBE_DISP_LIST_SIZE (ALIGN_TO_32( \
    BEGIN_INSTRUCTION_SIZE + \
    GET_VECTOR_INSTRUCTION_SIZE(3, sizeof(u8), NUM_CUBE_VERTICES) \
))

#define NUM_CROSS_VERTICES 8

#define CROSS_DISP_LIST_SIZE (ALIGN_TO_32( \
    BEGIN_INSTRUCTION_SIZE + \
    GET_VECTOR_INSTRUCTION_SIZE(3, sizeof(u8), NUM_CROSS_VERTICES) \
))

_Alignas(32) static u8 disp_list[CUBE_DISP_LIST_SIZE];

void block_selection_init(void) {
    GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
}

void block_selection_update(Mtx view) {
    guMtxConcat(view, model, model_view);
    GX_LoadPosMtxImm(model_view, MATRIX_INDEX);
}

void block_selection_draw(us_t now) {
    GX_SetNumTevStages(1);
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);

    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    u8 alpha = 0x5f + (sinf(now / 150000.0f) * 0x10);
    GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xff, 0xff, 0xff, alpha });
    GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
    GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
    GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    //

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);

    GX_SetCurrentMtx(MATRIX_INDEX);

    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GX_SetZCompLoc(GX_TRUE);

    GX_SetCullMode(cull_back ? GX_CULL_BACK : GX_CULL_NONE);

    if (!cull_back) {
        GX_SetCullMode(GX_CULL_NONE);
    }
    
    if (disp_list_size != 0) {
        GX_CallDispList(disp_list, disp_list_size);
    }

    GX_SetCullMode(GX_CULL_BACK);
}

void block_selection_handle_location(Mtx view, world_location_t location) {
    // Check if we have a new selected block
    if (has_last_block && location.bl_pos.x == last_block_pos.x && location.bl_pos.y == last_block_pos.y && location.bl_pos.z == last_block_pos.z && *location.bl_tp == last_block_type) {
        last_block_pos = location.bl_pos;
        last_block_type = *location.bl_tp;
        return;
    }

    has_last_block = true;
    last_block_pos = location.bl_pos;
    last_block_type = *location.bl_tp;

    s32vec3s chunk_pos = location.ch_pos;
    u8vec3s block_pos = location.bl_pos;

    guMtxIdentity(model);
    guMtxTransApply(model, model, chunk_pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK, chunk_pos.y * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK, chunk_pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK);
    guMtxConcat(view, model, model_view);
    
    GX_LoadPosMtxImm(model_view, MATRIX_INDEX);

    block_type_t block_type = *location.bl_tp;

    u8 px = block_pos.x * 4;
    u8 py = block_pos.y * 4;
    u8 pz = block_pos.z * 4;
    u8 pox = px + 4;
    u8 poy = py + 4;
    u8 poz = pz + 4;

    switch (block_type) {
        default:
            cull_back = true;

            memset(disp_list, 0, CUBE_DISP_LIST_SIZE);
            DCInvalidateRange(disp_list, CUBE_DISP_LIST_SIZE);
            
            GX_BeginDispList(disp_list, CUBE_DISP_LIST_SIZE);
            GX_Begin(GX_QUADS, VERTEX_FORMAT_INDEX, NUM_CUBE_VERTICES);

            GX_Position3u8(pox, poy, pz);
            GX_Position3u8(pox, py, pz);
            GX_Position3u8(pox, py,poz);
            GX_Position3u8(pox, poy,poz);
            GX_Position3u8(px, poy, pz);	// Top Left of the quad (top)
            GX_Position3u8(px, poy, poz);	// Top Right of the quad (top)
            GX_Position3u8(px, py, poz);	// Bottom Right of the quad (top)
            GX_Position3u8(px, py, pz);		// Bottom Left of the quad (top)
            GX_Position3u8(px, poy, poz);	// Bottom Left Of The Quad (Back)
            GX_Position3u8(px, poy, pz);	// Bottom Right Of The Quad (Back)
            GX_Position3u8(pox, poy, pz);	// Top Right Of The Quad (Back)
            GX_Position3u8(pox, poy, poz);	// Top Left Of The Quad (Back)
            GX_Position3u8(px, py, poz);		// Top Right Of The Quad (Front)
            GX_Position3u8(pox, py, poz);	// Top Left Of The Quad (Front)
            GX_Position3u8(pox, py, pz);	// Bottom Left Of The Quad (Front)
            GX_Position3u8(px, py, pz);	// Bottom Right Of The Quad (Front)
            GX_Position3u8(pox, py, poz);	// Top Right Of The Quad (Right)
            GX_Position3u8(px, py, poz);		// Top Left Of The Quad (Right)
            GX_Position3u8(px, poy, poz);	// Bottom Left Of The Quad (Right)
            GX_Position3u8(pox, poy, poz);	// Bottom Right Of The Quad (Right)
            GX_Position3u8(pox, py, pz);	// Top Right Of The Quad (Left)
            GX_Position3u8(pox, poy, pz);	// Top Left Of The Quad (Left)
            GX_Position3u8(px, poy, pz);	// Bottom Left Of The Quad (Left)
            GX_Position3u8(px, py, pz);	// Bottom Right Of The Quad (Left)
            
            GX_End();
            disp_list_size = GX_EndDispList();

            break;
        case block_type_air:
            disp_list_size = 0;
            break;
        case block_type_tall_grass:
            cull_back = false;
            
            memset(disp_list, 0, CROSS_DISP_LIST_SIZE);
            DCInvalidateRange(disp_list, CROSS_DISP_LIST_SIZE);

            GX_BeginDispList(disp_list, CROSS_DISP_LIST_SIZE);
            GX_Begin(GX_QUADS, VERTEX_FORMAT_INDEX, NUM_CROSS_VERTICES);

            GX_Position3u8(px, py, pz);
            GX_Position3u8(pox, py, poz);
            GX_Position3u8(pox, poy, poz);
            GX_Position3u8(px, poy, pz);
            GX_Position3u8(pox, py, pz);
            GX_Position3u8(px, py, poz);
            GX_Position3u8(px, poy, poz);
            GX_Position3u8(pox, poy, pz);

            GX_End();
            disp_list_size = GX_EndDispList();

            break;
    }
}