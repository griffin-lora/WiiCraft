#include "skybox.hpp"

using namespace game;

skybox::skybox(const math::matrix view, const camera& cam) {
    constexpr std::size_t vertex_count = 24;
    
    std::size_t disp_list_size = (
		4 + // GX_Begin
		vertex_count * 3 + // GX_Position3s8
		vertex_count * 2 + // GX_TexCoord2u8
		1 // GX_End
	);

    disp_list.resize(disp_list_size);

    disp_list.write_into([vertex_count]() {
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

    update(view, cam);
}

void skybox::update(const math::matrix view, const camera& cam) {
    tf.set_position(view, cam.position.x, cam.position.y, cam.position.z);
    tf.load(MAT);
}

void skybox::update_if_needed(const math::matrix view, const camera& cam) {
    if (cam.update_view) {
        update(view, cam);
    }
}

void skybox::draw() const {
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
    
    GX_SetCurrentMtx(MAT);

    disp_list.call();
}