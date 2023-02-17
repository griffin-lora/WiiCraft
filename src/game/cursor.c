#include "cursor.h"
#include "gfx/instruction_size.h"
#include "util.h"
#include <ogc/gx.h>

#define MATRIX_INDEX GX_PNMTX3
#define VERTEX_FORMAT_INDEX GX_VTXFMT3

#define VERTEX_COUNT 4

#define DISP_LIST_SIZE (ALIGN_TO_32_NEW( \
    GET_BEGIN_INSTRUCTION_SIZE(VERTEX_COUNT) + \
    GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), VERTEX_COUNT) + \
    GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), VERTEX_COUNT) \
))
_Alignas(32) static u8 disp_list[DISP_LIST_SIZE] = {
    0x83, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x30, 0x0, 0x1, 0x0, 0x30, 0x30, 0x1, 0x1, 0x0, 0x30, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

static Mtx model_view;

void cursor_init(void) {
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_POS, GX_POS_XY, GX_U8, 0);
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);
}

void cursor_update(u16 v_width, u16 v_height) {
    guMtxIdentity(model_view);
    guMtxTransApply(model_view, model_view, (v_width / 2) - 24.f, (v_height / 2) - 24.f, 0.0f);
    GX_LoadPosMtxImm(model_view, MATRIX_INDEX);
}

void cursor_draw(void) {
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
    
	GX_SetCurrentMtx(MATRIX_INDEX);
    GX_CallDispList(disp_list, DISP_LIST_SIZE);
}