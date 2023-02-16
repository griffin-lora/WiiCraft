#include "rendering.h"
#include <ogc/gx.h>

#define VERTEX_FORMAT_INDEX GX_VTXFMT2

void init_ui_rendering(void) {
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_POS, GX_POS_XY, GX_U16, 0);
	GX_SetVtxAttrFmt(VERTEX_FORMAT_INDEX, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);
}

void enter_ui_rendering(void) {
	GX_SetNumTevStages(1);
	GX_SetNumChans(1);

	GX_SetNumTexGens(1);

	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

	//

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
    GX_SetZCompLoc(GX_FALSE);
    GX_SetCullMode(GX_CULL_BACK);
}

void enter_text_rendering(void) {
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP3, GX_COLOR0A0);
}