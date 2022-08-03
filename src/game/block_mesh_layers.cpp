#include "block_mesh_layers.hpp"
#include "gfx/display_list.hpp"

using namespace game;
using namespace block_mesh_layer;

bool standard::is_single_sided() {
	return true;
}

std::size_t standard::get_chunk_display_list_size(std::size_t vert_count) {
    return
        gfx::get_begin_instruction_size(vert_count) +
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
        gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
}

void standard::write_chunk_vertex(const chunk_quad::vertex& vert) {
    GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
    GX_TexCoord2u8(vert.uv.x, vert.uv.y);
}

void standard::init_chunk_rendering() {
    GX_SetNumTevStages(2);
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);

	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);

	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE1, GX_PASSCLR);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 6);
}

static std::size_t get_tinted_chunk_display_list_size(std::size_t vert_count) {
    return
        gfx::get_begin_instruction_size(vert_count) +
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Color
        gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
}

static void write_tinted_chunk_vertex(const auto& vert) {
    GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
    GX_Color3u8(vert.color.r, vert.color.g, vert.color.b);
    GX_TexCoord2u8(vert.uv.x, vert.uv.y);
}

bool tinted::is_single_sided() {
	return true;
}

std::size_t tinted::get_chunk_display_list_size(std::size_t vert_count) {
    return get_tinted_chunk_display_list_size(vert_count);
}

void tinted::write_chunk_vertex(const chunk_quad::vertex& vert) {
    write_tinted_chunk_vertex(vert);
}

void tinted::init_chunk_rendering() {
    GX_SetNumTevStages(2);
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);

	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE1, GX_PASSCLR);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 6);
}

bool tinted_alpha::is_single_sided() {
	return true;
}

std::size_t tinted_alpha::get_chunk_display_list_size(std::size_t vert_count) {
    return get_tinted_chunk_display_list_size(vert_count);
}

void tinted_alpha::write_chunk_vertex(const chunk_quad::vertex& vert) {
    write_tinted_chunk_vertex(vert);
}

void tinted_alpha::init_chunk_rendering() {
    GX_SetAlphaCompare(GX_GEQUAL, 1, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_FALSE);
}

bool tinted_double_side_alpha::is_single_sided() {
	return false;
}

std::size_t tinted_double_side_alpha::get_chunk_display_list_size(std::size_t vert_count) {
    return get_tinted_chunk_display_list_size(vert_count);
}

void tinted_double_side_alpha::write_chunk_vertex(const chunk_quad::vertex& vert) {
    write_tinted_chunk_vertex(vert);
}

void tinted_double_side_alpha::init_chunk_rendering() {
	GX_SetCullMode(GX_CULL_NONE);
}

bool tinted_decal::is_single_sided() {
	return true;
}

std::size_t tinted_decal::get_chunk_display_list_size(std::size_t vert_count) {
    return
        gfx::get_begin_instruction_size(vert_count) +
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
        gfx::get_vector_instruction_size<3, u8>(vert_count) + // Color
        gfx::get_vector_instruction_size<2, u8>(vert_count) + // UV
        gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
}

void tinted_decal::write_chunk_vertex(const chunk_quad::vertex& vert) {
    GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
    GX_Color3u8(vert.color.r, vert.color.g, vert.color.b);
    GX_TexCoord2u8(vert.uv_0.x, vert.uv_0.y);
    GX_TexCoord2u8(vert.uv_1.x, vert.uv_1.y);
}

void tinted_decal::init_chunk_rendering() {
    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_TRUE);
	GX_SetCullMode(GX_CULL_BACK);

	GX_SetNumChans(1);
	GX_SetNumTexGens(2);
	GX_SetNumTevStages(3);

	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);

	// Decal
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	// Background
	GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_TEXC, GX_CC_ZERO, GX_CC_APREV, GX_CC_CPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_SetTevOrder(GX_TEVSTAGE2, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE2, GX_PASSCLR);
	GX_SetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX1, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 2);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 6);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_U8, 6);
}