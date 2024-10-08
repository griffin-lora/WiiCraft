#include "region_rendering.h"
#include "game/display_list.h"
#include "game/region.h"
#include "log.h"
#include <cglm/struct/affine.h>
#include <cglm/struct/mat4.h>
#include <ogc/gu.h>
#include <ogc/gx.h>

static void call_display_lists(const mat4s* view, size_t display_list_array_index) {
	REGION_TYPE_3D(region_render_info_t) render_infos = REGION_CAST_3D(region_render_info_t, region_render_infos);

	for (size_t x = 0; x < world_size; x++) {
		for (size_t y = 0; y < world_size; y++) {
			for (size_t z = 0; z < world_size; z++) {
				const region_render_info_t* info = &(*render_infos)[x][y][z];
				const region_display_list_array_t* display_list_array = &info->display_list_arrays[display_list_array_index];
				
				mat4s model;
				guMtxIdentity(model.raw);
				guMtxTransApply(model.raw, model.raw, (f32) (x * REGION_SIZE), (f32) (y * REGION_SIZE), (f32) (z * REGION_SIZE));
				
				mat4s model_view;
				guMtxConcat(view->raw, model.raw, model_view.raw);

				GX_LoadPosMtxImm(model_view.raw, REGION_MATRIX_INDEX);

				for (size_t i = 0; i < display_list_array->num_display_lists; i++) {
					const display_list_t* display_list = &display_list_array->display_lists[i];

					GX_CallDispList((void*) display_list->data, display_list->num_bytes);
				}
			}
		}
	}
}

void init_region_rendering(void) {
	GX_SetVtxAttrFmt(REGION_VERTEX_FORMAT_INDEX, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
	GX_SetVtxAttrFmt(REGION_VERTEX_FORMAT_INDEX, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);
}

void draw_regions(const mat4s* view) {
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

	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xff, 0xff, 0xff, 0xff }); // Set alpha

	call_display_lists(view, 0);
	call_display_lists(view, 1);
	GX_SetAlphaCompare(GX_GEQUAL, 1, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_FALSE);
	GX_SetCullMode(GX_CULL_NONE);
	call_display_lists(view, 2);
	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_TRUE);
	GX_SetCullMode(GX_CULL_BACK);
}