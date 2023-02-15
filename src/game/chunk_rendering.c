#include "chunk_rendering.h"
#include "pool.h"
#include "log.h"
#include <ogc/gx.h>

static void draw_pool(Mtx view, block_display_list_pool_t* pool) {
	const block_display_list_t* disp_lists = pool->disp_lists;
	block_display_list_chunk_t* chunks = pool->chunks;
	u16 head = pool->head;
	for (size_t i = 0; i < head; i++) {
		const block_display_list_t* disp_list = &disp_lists[i];

		Mtx model;
		Mtx model_view;
		guMtxIdentity(model);
		guMtxTransApply(model, model, disp_list->x, disp_list->y, disp_list->z);
		guMtxConcat(view, model, model_view);

		GX_LoadPosMtxImm(model_view, BLOCK_WORLD_MATRIX_INDEX);

		GX_CallDispList(chunks[disp_list->chunk_index], disp_list->size);
	}
}

void draw_block_display_lists(Mtx view) {
	GX_SetNumTevStages(2);
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);

	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE1, GX_PASSCLR);
	GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
	GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	//

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
	// Since the fractional size of the fixed point number is 4, it is equivalent to 1 unit = 16 pixels
	// I dont think that information is acccurate
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);

	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xff, 0xff, 0xff, 0xff }); // Set alpha

	draw_pool(view, &solid_display_list_pool);
	draw_pool(view, &transparent_display_list_pool);
	GX_SetAlphaCompare(GX_GEQUAL, 1, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_FALSE);
	GX_SetCullMode(GX_CULL_NONE);
	draw_pool(view, &transparent_double_sided_display_list_pool);
	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_TRUE);
	GX_SetCullMode(GX_CULL_BACK);
}