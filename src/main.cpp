#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <cmath>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <array>
#include "gfx.hpp"
#include "dbg.hpp"
#include "math.hpp"
#include "input.hpp"
#include "chrono.hpp"
#include "game/block_raycast.hpp"
#include "game/block_raycast.inl"
#include "game/camera.hpp"
#include "game/chunk_core.hpp"
#include "game/chunk_management.hpp"
#include "game/chunk_rendering.hpp"
#include "game/stored_chunk.hpp"
#include "game/logic.hpp"
#include "game/block_selection.hpp"
#include "game/cursor.hpp"
#include "game/skybox.hpp"
#include "game/character.hpp"
#include "game/rendering.hpp"
#include "game/water_overlay.hpp"
#include "game/debug_ui.hpp"
#include "common.hpp"

static constexpr f32 cam_rotation_speed = 1.80f;

static constexpr s32 chunk_generation_radius = 3;

static constexpr s32 chunk_erasure_radius = 4;

int main(int argc, char** argv) {

	gfx::console_state con;

	if (!fatInitDefault()) {
		printf("fatInitDefault() failed!\n");
		dbg::freeze();
	}

	gfx::texture chunk_tex;
	gfx::safe_load_from_file(chunk_tex, "data/textures/chunk.tpl");

	gfx::texture icons_tex;
	gfx::safe_load_from_file(icons_tex, "data/textures/icons.tpl");

	gfx::texture skybox_tex;
	gfx::safe_load_from_file(skybox_tex, "data/textures/skybox.tpl");

	gfx::texture font_tex;
	gfx::safe_load_from_file(font_tex, "data/textures/font.tpl");

	input::init(con.rmode->viWidth, con.rmode->viHeight);

	gfx::draw_state draw{ {0xFF, 0xFF, 0xFF, 0xFF} };

	input::set_resolution(draw.rmode->viWidth, draw.rmode->viHeight);

	gfx::set_filtering_mode(chunk_tex, GX_NEAR, GX_NEAR);
	gfx::set_filtering_mode(icons_tex, GX_NEAR, GX_NEAR);
	gfx::set_filtering_mode(font_tex, GX_NEAR, GX_NEAR);


	gfx::load(chunk_tex, GX_TEXMAP0);
	gfx::load(icons_tex, GX_TEXMAP1);
	gfx::load(skybox_tex, GX_TEXMAP2);
	gfx::load(font_tex, GX_TEXMAP3);

	math::matrix44 perspective_2d;
	guOrtho(perspective_2d, 0, 479, 0, 639, 0, 300);
	
	math::matrix view;
	math::matrix44 perspective_3d;

	game::character character = {
		.position = { 0.0f, 30.0f, 0.0f },
		.velocity = { 0.0f, 0.0f, 0.0f }
	};

	game::camera cam = {
		.position = {0.0f, 0.0f, -10.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.look = {0.0f, 0.0f, 1.0f},
		.aspect = (f32)((f32)draw.rmode->viWidth / (f32)draw.rmode->viHeight),
		.near_clipping_plane_distance = 0.1f,
		.far_clipping_plane_distance = 300.0f
	};
	character.update_camera(cam, chrono::get_current_us());
	std::optional<math::vector3s32> last_cam_chunk_pos;
	math::normalize(cam.look);

	game::update_look(cam);
	game::update_view(cam, view);

	game::update_perspective(cam, perspective_3d);

	bool first_frame = true;
	#ifdef PC_PORT
	u16 frame_count = 0;
	#endif

	game::chunk::map chunks;
	// This is a variable whose lifetime is bound to mesh updating functions normally. However, since it takes up quite a bit of memory, it is stored here.
	game::block::neighborhood_lookups block_nh_lookups(game::chunk::BLOCKS_COUNT);
	game::fill_block_neighborhood_lookups(block_nh_lookups);

	game::standard_quad_building_arrays quad_building_arrays;
	game::stored_chunk::map stored_chunks;

	// This is a variable whose lifetime is bound to the manage_chunks_around_camera function normally. However, reallocation is expensive, it is stored here.
	std::vector<math::vector3s32> chunk_positions_to_erase;
	game::chunk::pos_set chunk_positions_to_generate_blocks;
	game::chunk::pos_set chunk_positions_to_update_neighborhood_and_mesh;

	game::skybox skybox{view, cam};
	
	game::water_overlay water_overlay;
	game::debug_ui debug_ui;
	game::cursor cursor;

	game::block_selection bl_sel;

	gfx::set_z_buffer_mode(true, GX_LEQUAL, true);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);

	chrono::us total_block_gen_time = 0;
	chrono::us total_mesh_gen_time = 0;

	auto start = chrono::get_current_us();

	for (;;) {

        auto now = chrono::get_current_us();

		auto delta_time = now - start;
		f32 frame_delta = delta_time / 1000000.0f;
		f32 fps = 1000000.0f / delta_time;

		start = now;

		static constexpr s32 CHAN = 0;

		input::scan_pads();
		u32 buttons_down = input::get_buttons_down(CHAN);
		if (buttons_down & WPAD_BUTTON_HOME) { std::exit(CHAN); }
		u32 buttons_held = input::get_buttons_held(CHAN);

		game::update_camera_from_input(cam_rotation_speed, cam, frame_delta, buttons_held);

		auto pointer_pos = input::get_pointer_position(CHAN);
		cursor.update_from_pointer_position(draw.rmode->viWidth, draw.rmode->viHeight, pointer_pos);

    	auto gforce = input::get_gforce(CHAN);
		#ifndef PC_PORT
		expansion_t exp;
		if (input::scan_nunchuk(0, exp)) {
			const auto& nunchuk = exp.nunchuk;
			auto nunchuk_vector = input::get_nunchuk_vector(nunchuk);
			auto nunchuk_buttons_down = nunchuk.btns;

			character.handle_input(cam, now, frame_delta, gforce, nunchuk_vector, nunchuk_buttons_down, { nunchuk.gforce.x, nunchuk.gforce.y, nunchuk.gforce.z });
		}
		#else
		character.handle_input(cam, now, frame_delta, gforce, { 20.0f, 50.0f }, 0, gforce);
		#endif

		auto raycast = game::get_block_raycast(chunks, cam.position, cam.look * 10.0f, cam.position, cam.position + (cam.look * 10.0f), []<typename Bf>(game::bl_st st) {
			return Bf::get_selection_boxes(st);
		}, [](auto&) {});
		bl_sel.handle_raycast(view, quad_building_arrays, raycast);

		game::update_world_from_raycast_and_input(chunks, buttons_down, raycast);
		character.apply_physics(chunks, frame_delta);
		character.apply_velocity(frame_delta);
		character.update_camera(cam, now);

		game::manage_chunks_around_camera(chunk_erasure_radius, chunk_generation_radius, view, cam, last_cam_chunk_pos, chunks, stored_chunks, chunk_positions_to_erase, chunk_positions_to_generate_blocks, chunk_positions_to_update_neighborhood_and_mesh, total_block_gen_time);


		game::update_needed(view, perspective_3d, cam);

		game::update_chunks(block_nh_lookups, quad_building_arrays, chunks, total_mesh_gen_time);

		GX_LoadProjectionMtx(perspective_3d, GX_PERSPECTIVE);
		skybox.update_if_needed(view, cam);
		skybox.draw();

		GX_SetCurrentMtx(game::chunk::MAT);
		
		game::init_standard_rendering();
		game::draw_chunks_standard(view, cam, chunks);

		game::init_foliage_rendering();
		game::draw_chunks_foliage(chunks);

		game::init_water_rendering();
		game::draw_chunks_water(chunks);
		
		bl_sel.update_if_needed(view, cam);
		bl_sel.draw(raycast);
		
		GX_LoadProjectionMtx(perspective_2d, GX_ORTHOGRAPHIC);
		game::init_ui_rendering();
		water_overlay.draw(cam, chunks);
		cursor.draw();

		game::init_text_rendering();
		debug_ui.draw(character.position, cam.look, total_block_gen_time, total_mesh_gen_time, std::floor(fps));

		game::reset_update_params(cam);

		// Frame drawing is done at this point.
		gfx::copy_framebuffer(draw.frame_buffers[draw.fb_index], true);

		gfx::draw_done();

		VIDEO_SetNextFramebuffer(draw.frame_buffers[draw.fb_index]);
		if (first_frame) {
			first_frame = false;
			VIDEO_SetBlack(FALSE);
		}
		VIDEO_Flush();
		VIDEO_WaitVSync();
		draw.fb_index ^= 1;
		
		#ifdef PC_PORT
		if (++frame_count == 1200) {
			std::printf("BGT: %ld\nMGT: %ld\n", total_block_gen_time, total_mesh_gen_time);
			std::exit(0);
		}
		#endif
	}
}
