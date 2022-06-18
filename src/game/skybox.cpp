#include "skybox.hpp"

using namespace game;

void game::init(skybox& skybox) {
    constexpr std::size_t vertex_count = 24;
    
    std::size_t disp_list_size = (
		4 + // GX_Begin
		vertex_count * 3 + // GX_Position3s8
		vertex_count * 2 + // GX_TexCoord2u8
		1 // GX_End
	);

    skybox.disp_list.resize(disp_list_size);

    skybox.disp_list.write_into([&skybox]() {
        constexpr s8 n = -0x80;
        constexpr s8 p = 0x7f;

        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        // Front (+x)

        GX_Position3s8(p, n, n);
        GX_TexCoord2u8(1, 2);

        GX_Position3s8(p, p, n);
        GX_TexCoord2u8(1, 1);

        GX_Position3s8(p, p, p);
        GX_TexCoord2u8(2, 1);

        GX_Position3s8(p, n, p);
        GX_TexCoord2u8(2, 2);

        // Back (-x)

        GX_Position3s8(n, p, p);
        GX_TexCoord2u8(1, 1);

        GX_Position3s8(n, p, n);
        GX_TexCoord2u8(2, 1);

        GX_Position3s8(n, n, n);
        GX_TexCoord2u8(2, 2);

        GX_Position3s8(n, n, p);
        GX_TexCoord2u8(1, 2);

        // Top (+y)

        GX_Position3s8(n, p, n);
        GX_TexCoord2u8(0, 0);

        GX_Position3s8(n, p, p);
        GX_TexCoord2u8(1, 0);

        GX_Position3s8(p, p, p);
        GX_TexCoord2u8(1, 1);

        GX_Position3s8(p, p, n);
        GX_TexCoord2u8(0, 1);

        // Bottom (-y)

        GX_Position3s8(n, n, n);
        GX_TexCoord2u8(1, 2);

        GX_Position3s8(p, n, n);
        GX_TexCoord2u8(2, 2);

        GX_Position3s8(p, n, p);
        GX_TexCoord2u8(2, 3);

        GX_Position3s8(n, n, p);
        GX_TexCoord2u8(1, 3);

        // Right (+z)

        GX_Position3s8(n, n, p);
        GX_TexCoord2u8(1, 2);

        GX_Position3s8(p, n, p);
        GX_TexCoord2u8(2, 2);

        GX_Position3s8(p, p, p);
        GX_TexCoord2u8(2, 1);

        GX_Position3s8(n, p, p);
        GX_TexCoord2u8(1, 1);

        // Left (-z)

        GX_Position3s8(p, p, n);
        GX_TexCoord2u8(1, 1);

        GX_Position3s8(p, n, n);
        GX_TexCoord2u8(1, 2);

        GX_Position3s8(n, n, n);
        GX_TexCoord2u8(2, 2);

        GX_Position3s8(n, p, n);
        GX_TexCoord2u8(2, 1);


        GX_End();
    });
}

void game::update_skybox(math::matrix view, const camera& cam, skybox& skybox) {
    skybox.tf.set_position(view, cam.position.x, cam.position.y, cam.position.z);
}

static void init_drawing() {
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
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S8, 0);
	// Since the fractional size of the fixed point number is 3, it is equivalent to 1 unit = 2 pixels
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 3);
}

void game::draw_skybox(gfx::texture& tex, skybox& skybox) {
    init_drawing();
    gfx::load(tex);
    skybox.tf.load(GX_PNMTX3);
    skybox.disp_list.call();
}