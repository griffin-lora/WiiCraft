#include "chunk_rendering.hpp"
#include "gfx.hpp"
#include "gfx/display_list_new.hpp"
#include "pool.hpp"

using namespace game;

static void set_alpha(u8 alpha) {
	GX_SetTevColor(GX_TEVREG1, { 0xff, 0xff, 0xff, alpha });
}

void game::draw_chunks(const math::matrix view, const camera& cam, chunk::map& chunks) {
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

	if (cam.update_view) { // This may be a mostly useless micro-optimization, matrix multiplication vs a branch ehhh (we are using a very expensive for loop though so yeah)
		for (auto& [ pos, chunk ] : chunks) {
			set_alpha(chunk.alpha);

			chunk.tf.update_model_view(view);
			chunk.tf.load(chunk::mat);

			for (pool_display_list_t disp_list : chunk.solid_display_lists) {
				GX_CallDispList(disp_list.chunk, disp_list.size);
			}
		}
		for (auto& [ pos, chunk ] : chunks) {
			chunk.tf.load(chunk::mat);

			for (pool_display_list_t disp_list : chunk.transparent_display_lists) {
				GX_CallDispList(disp_list.chunk, disp_list.size);
			}
		}
		GX_SetAlphaCompare(GX_GEQUAL, 1, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_FALSE);
		GX_SetCullMode(GX_CULL_NONE);
		for (auto& [ pos, chunk ] : chunks) {
			chunk.tf.load(chunk::mat);

			for (pool_display_list_t disp_list : chunk.transparent_double_sided_display_lists) {
				GX_CallDispList(disp_list.chunk, disp_list.size);
			}
		}
		GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_TRUE);
		GX_SetCullMode(GX_CULL_BACK);
	} else {
		for (auto& [ pos, chunk ] : chunks) {
			set_alpha(chunk.alpha);

			chunk.tf.load(chunk::mat);
			for (pool_display_list_t disp_list : chunk.solid_display_lists) {
				GX_CallDispList(disp_list.chunk, disp_list.size);
			}
		}

		for (auto& [ pos, chunk ] : chunks) {
			chunk.tf.load(chunk::mat);

			for (pool_display_list_t disp_list : chunk.transparent_display_lists) {
				GX_CallDispList(disp_list.chunk, disp_list.size);
			}
		}
		GX_SetAlphaCompare(GX_GEQUAL, 1, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_FALSE);
		GX_SetCullMode(GX_CULL_NONE);
		for (auto& [ pos, chunk ] : chunks) {
			chunk.tf.load(chunk::mat);

			for (pool_display_list_t disp_list : chunk.transparent_double_sided_display_lists) {
				GX_CallDispList(disp_list.chunk, disp_list.size);
			}
		}
		GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_TRUE);
		GX_SetCullMode(GX_CULL_BACK);
	}
}