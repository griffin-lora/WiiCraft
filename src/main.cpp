#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
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
#include "game/chunk_mesh_generation.hpp"
#include "common.hpp"
#include "log.hpp"
#include "pool.h"
#include "game/block_world_management.hpp"

static constexpr f32 cam_rotation_speed = 1.80f;

static constexpr s32 chunk_generation_radius = 6;

static constexpr s32 chunk_erasure_radius = 7;

int main(int argc, char** argv) {
	if (!log_init()) {
		return 1;
	}

	lprintf("Log started\n");

	if (!gfx_init()) {
		return 1;
	}

	pool_init();

	input::init(rmode->viWidth, rmode->viHeight);

	input::set_resolution(rmode->viWidth, rmode->viHeight);

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
		.aspect = (f32)((f32)rmode->viWidth / (f32)rmode->viHeight),
		.near_clipping_plane_distance = 0.1f,
		.far_clipping_plane_distance = 300.0f
	};
	character.update_camera(cam, 0);
	std::optional<math::vector3s32> last_cam_chunk_pos;
	math::normalize(cam.look);

	game::update_look(cam);
	game::update_view(cam, view);

	game::update_perspective(cam, perspective_3d);

	#ifdef PC_PORT
	u16 frame_count = 0;
	#endif

	// game::chunk::map chunks;

	// game::stored_chunk::map stored_chunks;

	skybox_init(view, cam.position.x, cam.position.y, cam.position.z);
	
	game::water_overlay water_overlay;
	game::debug_ui debug_ui;
	
	cursor_init();

	block_selection_init();

	GX_SetZMode(true, GX_LEQUAL, true);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);

	math::vector3u16 last_wpad_accel = { 512, 512, 512 };
	math::vector3u16 last_nunchuk_accel = { 512, 512, 512 };

	chrono::us total_block_gen_time = 0;
	chrono::us total_mesh_gen_time = 0;
	chrono::us last_mesh_gen_time = 0;

	chrono::us_tp<s64> program_start = chrono::get_current_us();
	chrono::us start = 0;

	vec3_s32_t last_corner_pos = { floorf(cam.position.x / 16.0f), floorf(cam.position.y / 16.0f), floorf(cam.position.z / 16.0f) };

	init_block_world(last_corner_pos);

	for (;;) {
        chrono::us now = chrono::get_current_us() - program_start;

		chrono::us delta_time = now - start;
		f32 frame_delta = delta_time / 1000000.0f;
		f32 fps = 1000000.0f / delta_time;

		start = now;

		s32 chan = 0;

		input::scan_pads();
		u32 buttons_down = input::get_buttons_down(chan);
		if (buttons_down & WPAD_BUTTON_HOME) {
			lprintf("BGT: %d\nMGT: %d\nMGL: %d\nLog ended\n", total_block_gen_time, total_mesh_gen_time, last_mesh_gen_time);
			log_term();
			std::exit(0);
		}
		u32 buttons_held = input::get_buttons_held(chan);

		game::update_camera_from_input(cam_rotation_speed, cam, frame_delta, buttons_held);

		auto pointer_pos = input::get_pointer_position(chan);
		cursor_update(rmode->viWidth, rmode->viHeight);

		#ifndef PC_PORT
    	auto wpad_accel = input::get_accel(chan);
		expansion_t exp;
		if (input::scan_nunchuk(0, exp)) {
			const auto& nunchuk = exp.nunchuk;
			auto nunchuk_vector = input::get_nunchuk_vector(nunchuk);
			auto nunchuk_buttons_down = nunchuk.btns;

			math::vector3u16 nunchuk_accel = { nunchuk.accel.x, nunchuk.accel.y, nunchuk.accel.z };

			character.handle_input(cam, last_wpad_accel, last_nunchuk_accel, now, frame_delta, wpad_accel, nunchuk_vector, nunchuk_buttons_down, nunchuk_accel);

			last_nunchuk_accel = nunchuk_accel;
			// character.velocity.y = input::get_plus_minus_input_scalar(buttons_held) * 15.0f;
		}
		last_wpad_accel = wpad_accel;
		#else
		character.handle_input(cam, { 0, 0, 0 }, { 0, 0, 0 }, now, frame_delta, { 512, 512, 512 }, { 96.0f, 96.0f }, 0, { 512, 512, 512 });
		#endif

		auto raycast_dir = game::get_raycast_direction_from_pointer_position(rmode->viWidth, rmode->viHeight, cam, pointer_pos);
		// auto raycast = get_block_raycast(chunks, cam.position, raycast_dir * 10.0f, cam.position, cam.position + (raycast_dir * 10.0f), { 0, 0, 0 }, block_box_type_selection);
		// block_selection_handle_raycast(view, raycast);

		// game::update_world_from_raycast_and_input(chunks, buttons_down, raycast);
		// character.apply_physics(chunks, frame_delta);
		character.apply_velocity(frame_delta);
		character.update_camera(cam, now);

		vec3_s32_t corner_pos = { floorf(cam.position.x / 16.0f), floorf(cam.position.y / 16.0f), floorf(cam.position.z / 16.0f) };
		// if (cam.update_view) {
			manage_block_world(last_corner_pos, corner_pos);
		// }
		handle_world_queues(corner_pos);
		last_corner_pos = corner_pos;

		game::update_needed(view, perspective_3d, cam);

		if (cam.update_view) {
			skybox_update(view, cam.position.x, cam.position.y, cam.position.z);
			block_selection_update(view);
		}
		debug_ui.update(buttons_down);

		GX_LoadProjectionMtx(perspective_3d, GX_PERSPECTIVE);
		skybox_draw();

		GX_SetCurrentMtx(game::chunk::mat);

		draw_block_display_lists(view);
		
		// if (raycast.success) {
		// 	block_selection_draw(now);
		// }
		
		GX_LoadProjectionMtx(perspective_2d, GX_ORTHOGRAPHIC);
		game::init_ui_rendering();
		// water_overlay.draw(cam, chunks);
		cursor_draw();

		game::init_text_rendering();
		debug_ui.draw(character.position, cam.look, total_block_gen_time, total_mesh_gen_time, last_mesh_gen_time, std::ceil(fps));

		game::reset_update_params(cam);

		gfx_update_video();
		
		#ifdef PC_PORT
		if (++frame_count == 1200) {
			std::printf("BGT: %ld\nMGT: %ld\n", total_block_gen_time, total_mesh_gen_time);
			std::exit(0);
		}
		#endif
	}
}
