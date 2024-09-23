#include "cursor.h"
#include "gfx/instruction_size.h"
#include "util.h"
#include <cglm/struct/affine.h>
#include <cglm/struct/cam.h>
#include <ogc/gu.h>
#include <ogc/gx.h>

#define MATRIX_INDEX GX_PNMTX3
#define VERTEX_FORMAT_INDEX GX_VTXFMT3

#define NUM_VERTICES 4

#define DISP_LIST_SIZE (ALIGN_TO_32_NEW( \
    BEGIN_INSTRUCTION_SIZE + \
    GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), NUM_VERTICES) + \
    GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), NUM_VERTICES) \
))
alignas(32) static u8 disp_list[DISP_LIST_SIZE] = {
    0x83, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x30, 0x0, 0x1, 0x0, 0x30, 0x30, 0x1, 0x1, 0x0, 0x30, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

void cursor_init(void) {
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_POS, GX_POS_XY, GX_U8, 0);
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);
}

void cursor_update(u16 v_width, u16 v_height) {
    mat4s model;
    guMtxIdentity(model.raw);
    guMtxTransApply(model.raw, model.raw, ((f32) v_width / 2.0f) - 24.f, ((f32) v_height / 2.0f) - 24.f, 0.0f);
    GX_LoadPosMtxImm(model.raw, MATRIX_INDEX);
}

void cursor_draw(void) {
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
    
	GX_SetCurrentMtx(MATRIX_INDEX);
    GX_CallDispList(disp_list, DISP_LIST_SIZE);
}