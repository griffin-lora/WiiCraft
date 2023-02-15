#include "skybox.h"
#include "gfx/instruction_size.h"
#include "util.h"
#include <ogc/gx.h>
#include <stdalign.h>

#define MATRIX_INDEX GX_PNMTX0

#define VERTEX_COUNT 24

#define DISP_LIST_SIZE (ALIGN_TO_32( \
    GET_BEGIN_INSTRUCTION_SIZE(VERTEX_COUNT) + \
    GET_VECTOR_INSTRUCTION_SIZE(3, sizeof(s8), VERTEX_COUNT) + \
    GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), VERTEX_COUNT) \
))
_Alignas(32) static u8 disp_list[DISP_LIST_SIZE];
static size_t disp_list_size;

static Mtx model;
static Mtx model_view;

#define N -0x80
#define P 0x7f

void skybox_init(Mtx view, vec3s cam_pos) {
    GX_BeginDispList(disp_list, DISP_LIST_SIZE);

    GX_Begin(GX_QUADS, GX_VTXFMT0, VERTEX_COUNT);

    // Front (+x)

    GX_Position3s8(P, N, N);
    GX_TexCoord2u8(1, 2);

    GX_Position3s8(P, P, N);
    GX_TexCoord2u8(1, 1);

    GX_Position3s8(P, P, P);
    GX_TexCoord2u8(2, 1);

    GX_Position3s8(P, N, P);
    GX_TexCoord2u8(2, 2);

    // Back (-x)

    GX_Position3s8(N, P, P);
    GX_TexCoord2u8(1, 1);

    GX_Position3s8(N, P, N);
    GX_TexCoord2u8(2, 1);

    GX_Position3s8(N, N, N);
    GX_TexCoord2u8(2, 2);

    GX_Position3s8(N, N, P);
    GX_TexCoord2u8(1, 2);

    // ToP (+y)

    GX_Position3s8(N, P, N);
    GX_TexCoord2u8(0, 0);

    GX_Position3s8(N, P, P);
    GX_TexCoord2u8(1, 0);

    GX_Position3s8(P, P, P);
    GX_TexCoord2u8(1, 1);

    GX_Position3s8(P, P, N);
    GX_TexCoord2u8(0, 1);

    // Bottom (-y)

    GX_Position3s8(N, N, N);
    GX_TexCoord2u8(1, 2);

    GX_Position3s8(P, N, N);
    GX_TexCoord2u8(2, 2);

    GX_Position3s8(P, N, P);
    GX_TexCoord2u8(2, 3);

    GX_Position3s8(N, N, P);
    GX_TexCoord2u8(1, 3);

    // Right (+z)

    GX_Position3s8(N, N, P);
    GX_TexCoord2u8(1, 2);

    GX_Position3s8(P, N, P);
    GX_TexCoord2u8(2, 2);

    GX_Position3s8(P, P, P);
    GX_TexCoord2u8(2, 1);

    GX_Position3s8(N, P, P);
    GX_TexCoord2u8(1, 1);

    // Left (-z)

    GX_Position3s8(P, P, N);
    GX_TexCoord2u8(1, 1);

    GX_Position3s8(P, N, N);
    GX_TexCoord2u8(1, 2);

    GX_Position3s8(N, N, N);
    GX_TexCoord2u8(2, 2);

    GX_Position3s8(N, P, N);
    GX_TexCoord2u8(2, 1);

    GX_End();
    disp_list_size = GX_EndDispList();

    skybox_update(view, cam_pos);
}

void skybox_update(Mtx view, vec3s cam_pos) {
    guMtxIdentity(model);
    guMtxTransApply(model, model, cam_pos.x, cam_pos.y, cam_pos.z);
    guMtxConcat(view, model, model_view);
    GX_LoadPosMtxImm(model_view, MATRIX_INDEX);
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

	//

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// GX_VTXFMT0 is for standard geometry
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S8, 0);
	// Since the fractional size of the fixed point number is 3, it is equivalent to 1 unit = 2 pixels
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 3);

    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

    GX_SetZCompLoc(GX_TRUE);
    GX_SetCullMode(GX_CULL_BACK);

    //
    
    GX_SetCurrentMtx(MATRIX_INDEX);

    GX_CallDispList(disp_list, disp_list_size);
}