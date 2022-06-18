#include "cursor.hpp"

using namespace game;

void game::init(cursor& cursor) {
    constexpr std::size_t vertex_count = 4;
    
    std::size_t disp_list_size = (
		4 + // GX_Begin
		vertex_count * 2 + // GX_Position2u8
		vertex_count * 2 + // GX_TexCoord2u8
		1 // GX_End
	);

    cursor.disp_list.resize(disp_list_size);

    cursor.disp_list.write_into([&cursor]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        GX_Position2u8(0, 0);
        GX_TexCoord2u8(0, 0);
        GX_Position2u8(100, 0);
        GX_TexCoord2u8(1, 0);
        GX_Position2u8(100, 100);
        GX_TexCoord2u8(1, 1);
        GX_Position2u8(0, 100);
        GX_TexCoord2u8(0, 1);

        GX_End();
    });
}

static void init_cursor_drawing() {
    // set number of rasterized color channels
	GX_SetNumChans(1);

	//set number of textures to generate
	GX_SetNumTexGens(1);

	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	//

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// GX_VTXFMT0 is for standard cube geometry
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_U8, 0);
	// Since the fractional size of the fixed point number is 4, it is equivalent to 1 unit = 16 pixels
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);
}

void game::draw_cursor(gfx::texture& texture, math::matrix model_view, cursor& cursor) {
    init_cursor_drawing();

    gfx::load(texture);

    GX_LoadPosMtxImm(model_view, GX_PNMTX3);

    cursor.disp_list.call();
}