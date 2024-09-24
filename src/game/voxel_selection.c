#include "voxel_selection.h"
#include "game/region.h"
#include "game/region_management.h"
#include "game_math.h"
#include "gfx/instruction_size.h"
#include "log.h"
#include "util.h"
#include "voxel.h"
#include <cglm/struct/affine.h>
#include <cglm/struct/mat4.h>
#include <ogc/gu.h>
#include <ogc/gx.h>
#include <ogc/cache.h>
#include <string.h>
#include <stdbool.h>
#include <stdalign.h>

#define MATRIX_INDEX GX_PNMTX4
#define VERTEX_FORMAT_INDEX GX_VTXFMT4

static bool has_last_selection = false;
static u32vec3s last_voxel_local_pos;
static voxel_type_t last_voxel_type;

static mat4s model;

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

alignas(32) static u8 disp_list[CUBE_DISP_LIST_SIZE];

void voxel_selection_init(void) {
    GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
}

void voxel_selection_update_view(const mat4s* view) {
    mat4s model_view;
    guMtxConcat(view->raw, model.raw, model_view.raw);
    GX_LoadPosMtxImm(model_view.raw, MATRIX_INDEX);
}

void voxel_selection_draw(us_t now) {
    GX_SetNumTevStages(1);
    GX_SetNumChans(1);
    GX_SetNumTexGens(0);

    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

    u8 alpha = 0x5f + (u8) (sinf((f32) now / 150000.0f) * 0x10);
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
        GX_CallDispList(disp_list, (u32) disp_list_size);
    }

    GX_SetCullMode(GX_CULL_BACK);
}

void voxel_selection_update(const mat4s* view, s32vec3s voxel_world_pos) {
    s32vec3s region_pos = get_region_position_from_voxel_world_position(voxel_world_pos);
    u32vec3s voxel_local_pos = get_voxel_local_position_from_voxel_world_position(voxel_world_pos);

    REGION_TYPE_3D(const voxel_type_array_t*) voxel_type_arrays = REGION_CAST_3D(const voxel_type_array_t*, region_voxel_type_arrays);
    u32vec3s region_rel_pos = get_region_relative_position(region_pos);

    voxel_type_t voxel_type = (*voxel_type_arrays)[region_rel_pos.x][region_rel_pos.y][region_rel_pos.z]->types[voxel_local_pos.x][voxel_local_pos.y][voxel_local_pos.z];

    // Check if we have a new selected voxel
    if (has_last_selection && voxel_local_pos.x == last_voxel_local_pos.x && voxel_local_pos.y == last_voxel_local_pos.y && voxel_local_pos.z == last_voxel_local_pos.z && voxel_type == last_voxel_type) {
        last_voxel_local_pos = voxel_local_pos;
        last_voxel_type = voxel_type;
        return;
    }

    has_last_selection = true;
    last_voxel_local_pos = voxel_local_pos;
    last_voxel_type = voxel_type;
    
    guMtxIdentity(model.raw);
    guMtxTransApply(model.raw, model.raw, (f32) region_pos.x * REGION_SIZE, (f32) region_pos.y * REGION_SIZE, (f32) region_pos.z * REGION_SIZE);

    voxel_selection_update_view(view);

    u8 px = (u8) voxel_local_pos.x * 4;
    u8 py = (u8) voxel_local_pos.y * 4;
    u8 pz = (u8) voxel_local_pos.z * 4;
    u8 pox = px + 4;
    u8 poy = py + 4;
    u8 poz = pz + 4;

    switch (voxel_type) {
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
        case voxel_type_air:
            disp_list_size = 0;
            break;
        case voxel_type_tall_grass:
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