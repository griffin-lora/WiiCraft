#include "skybox.h"
#include "gfx/instruction_size.h"
#include "util.h"
#include <cglm/struct/affine.h>
#include <cglm/struct/mat4.h>
#include <ogc/gx.h>
#include <stdalign.h>

#define MATRIX_INDEX GX_PNMTX0
#define VERTEX_FORMAT_INDEX GX_VTXFMT0

#define NUM_VERTICES 24

#define DISP_LIST_SIZE (ALIGN_TO_32_NEW( \
    BEGIN_INSTRUCTION_SIZE + \
    GET_VECTOR_INSTRUCTION_SIZE(3, sizeof(s8), NUM_VERTICES) + \
    GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), NUM_VERTICES) \
))
alignas(32) static u8 disp_list[DISP_LIST_SIZE] = {
    0x80, 0x0, 0x18, 0x1, 0xff, 0xff, 0x1, 0x2, 0x1, 0x1, 0xff, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x1, 0x1, 0xff, 0x1, 0x2, 0x2, 0xff, 0x1, 0x1, 0x1, 0x1, 0xff, 0x1, 0xff, 0x2, 0x1, 0xff, 0xff, 0xff, 0x2, 0x2, 0xff, 0xff, 0x1, 0x1, 0x2, 0xff, 0x1, 0xff, 0x0, 0x0, 0xff, 0x1, 0x1, 0x1, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0xff, 0x0, 0x1, 0xff, 0xff, 0xff, 0x1, 0x2, 0x1, 0xff, 0xff, 0x2, 0x2, 0x1, 0xff, 0x1, 0x2, 0x3, 0xff, 0xff, 0x1, 0x1, 0x3, 0xff, 0xff, 0x1, 0x1, 0x2, 0x1, 0xff, 0x1, 0x2, 0x2, 0x1, 0x1, 0x1, 0x2, 0x1, 0xff, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0xff, 0x1, 0x1, 0x1, 0xff, 0xff, 0x1, 0x2, 0xff, 0xff, 0xff, 0x2, 0x2, 0xff, 0x1, 0xff, 0x2, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0
};

void skybox_init(const mat4s* view, vec3s cam_pos) {
	// Skybox Vtxfmt init
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_POS, GX_POS_XYZ, GX_S8, 0);
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_TEX0, GX_TEX_ST, GX_U8, 3);

    skybox_update(view, cam_pos);
}

void skybox_update(const mat4s* view, vec3s cam_pos) {
    mat4s model = glms_translate_make(cam_pos);
    mat4s model_view = glms_mat4_mul(*view, model);
    model_view = glms_mat4_transpose(model_view);
    GX_LoadPosMtxImm(model_view.raw, MATRIX_INDEX);
}

void skybox_draw(void) {
    GX_SetNumTevStages(1);
    // set number of rasterized color channels
	GX_SetNumChans(1);

	//set number of textures to generate
	GX_SetNumTexGens(1);

	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP2, GX_COLOR0A0);

    GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

    GX_SetZCompLoc(GX_TRUE);
    GX_SetCullMode(GX_CULL_BACK);

    //
    
    GX_SetCurrentMtx(MATRIX_INDEX);

	GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);

    GX_CallDispList(disp_list, DISP_LIST_SIZE);

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
}