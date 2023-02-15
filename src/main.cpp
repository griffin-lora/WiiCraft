#include "gfx.hpp"
#include "dbg.hpp"
#include "game_math.hpp"
#include "input.hpp"
#include "chrono.hpp"
#include "game/block_raycast.hpp"
#include "game/camera.hpp"
#include "game/chunk_rendering.hpp"
#include "game/logic.hpp"
#include "game/block_selection.hpp"
#include "game/cursor.hpp"
#include "game/skybox.hpp"
#include "game/character.hpp"
#include "game/rendering.hpp"
#include "game/water_overlay.hpp"
#include "game/debug_ui.hpp"
#include "log.hpp"
#include "pool.hpp"
#include "game/block_world_management.hpp"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <array>

static constexpr f32 cam_rotation_speed = 1.80f;

static constexpr s32 chunk_generation_radius = 6;

static constexpr s32 chunk_erasure_radius = 7;

static s32vec3s corner_pos_offset = {{ -3, -2, -3 }};

int main(int argc, char** argv) {
	if (!log_init()) {
		return 1;
	}

	lprintf("Log started\n");

	if (!gfx_init()) {
		return 1;
	}

	pool_init();

	input_init(rmode->viWidth, rmode->viHeight);

	Mtx44 perspective_2d;
	guOrtho(perspective_2d, 0, 479, 0, 639, 0, 300);
	
	Mtx view;
	Mtx44 perspective_3d;

	character_position = { 0.0f, 30.0f, 0.0f };
	character_velocity = { 0.0f, 0.0f, 0.0f };

	game::camera cam = {
		.position = {0.0f, 0.0f, -10.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.look = {0.0f, 0.0f, 1.0f},
		.aspect = (f32)((f32)rmode->viWidth / (f32)rmode->viHeight),
		.near_clipping_plane_distance = 0.1f,
		.far_clipping_plane_distance = 300.0f
	};
	game::update_camera(cam, 0);
	glm_vec3_normalize(cam.look.raw);

	game::update_look(cam);
	game::update_view(cam, view);

	game::update_perspective(cam, perspective_3d);

	#ifdef PC_PORT
	u16 frame_count = 0;
	#endif

	skybox_init(view, cam.position);
	
	water_overlay_init();
	game::debug_ui debug_ui;
	
	cursor_init();

	block_selection_init();

	GX_SetZMode(true, GX_LEQUAL, true);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);

	vec3w_t last_wpad_accel = { 512, 512, 512 };
	vec3w_t last_nunchuk_accel = { 512, 512, 512 };

	s64 program_start = get_current_us();
	us_t start = 0;
	
	vec3s div_pos = glms_vec3_scale(cam.position, 1.0f/(f32)NUM_ROW_BLOCKS_PER_BLOCK_CHUNK);
	s32vec3s last_corner_pos = {{ (s32)floorf(div_pos.raw[0]), (s32)floorf(div_pos.raw[1]), (s32)floorf(div_pos.raw[2]) }};
	glm_ivec3_add(last_corner_pos.raw, corner_pos_offset.raw, last_corner_pos.raw);

	init_block_world(last_corner_pos);

	for (;;) {
        us_t now = get_current_us() - program_start;

		us_t delta_time = now - start;
		f32 frame_delta = delta_time / 1000000.0f;
		f32 fps = 1000000.0f / delta_time;

		start = now;

		s32 chan = 0;

		WPAD_ScanPads();
		u32 buttons_down = WPAD_ButtonsDown(chan);
		if (buttons_down & WPAD_BUTTON_HOME) {
			lprintf("BGT: %d\nMGT: %d\nMGL: %d\nLog ended\n", total_block_gen_time, total_mesh_gen_time, last_mesh_gen_time);
			log_term();
			std::exit(0);
		}
		u32 buttons_held = WPAD_ButtonsHeld(chan);

		game::update_camera_from_input(cam_rotation_speed, cam, frame_delta, buttons_held);

		vec3s div_pos = glms_vec3_scale(cam.position, 1.0f/(f32)NUM_ROW_BLOCKS_PER_BLOCK_CHUNK);
		s32vec3s corner_pos = {{ (s32)floorf(div_pos.raw[0]), (s32)floorf(div_pos.raw[1]), (s32)floorf(div_pos.raw[2]) }};
		glm_ivec3_add(corner_pos.raw, corner_pos_offset.raw, corner_pos.raw);
		if (last_corner_pos.raw[0] != corner_pos.raw[0] || last_corner_pos.raw[1] != corner_pos.raw[1] || last_corner_pos.raw[2] != corner_pos.raw[2]) {
			manage_block_world(last_corner_pos, corner_pos);
		}
		handle_world_flag_processing(corner_pos);
		last_corner_pos = corner_pos;

		cursor_update(rmode->viWidth, rmode->viHeight);

		#ifndef PC_PORT
    	vec3w_t wpad_accel;
		WPAD_Accel(chan, &wpad_accel);
		expansion_t exp;
		WPAD_Expansion(chan, &exp);
		if (exp.type == WPAD_EXP_NUNCHUK) {
			const auto& nunchuk = exp.nunchuk;
			auto nunchuk_vector = get_nunchuk_vector(&nunchuk);
			auto nunchuk_buttons_down = nunchuk.btns;

			vec3w_t nunchuk_accel = { nunchuk.accel.x, nunchuk.accel.y, nunchuk.accel.z };

			character_handle_input(cam.look, last_wpad_accel, last_nunchuk_accel, now, frame_delta, wpad_accel, nunchuk_vector, nunchuk_buttons_down, nunchuk_accel);

			last_nunchuk_accel = nunchuk_accel;
			// character.velocity.y = get_plus_minus_input_scalar(buttons_held) * 15.0f;
		}
		last_wpad_accel = wpad_accel;
		#else
		character_handle_input(cam.look, { 0, 0, 0 }, { 0, 0, 0 }, now, frame_delta, { 512, 512, 512 }, { 96.0f, 96.0f }, 0, { 512, 512, 512 });
		#endif

		// auto raycast_dir = get_raycast_direction_from_pointer_position(rmode->viWidth, rmode->viHeight, cam, pointer_pos);
		vec3s raycast_dir = cam.look;
		block_raycast_wrap_t raycast = get_block_raycast(corner_pos, cam.position, glms_vec3_scale(raycast_dir, 10.0f), cam.position, glms_vec3_add(cam.position, glms_vec3_scale(raycast_dir, 10.0f)), { 0, 0, 0 }, block_box_type_selection);

		if (raycast.success) {
			block_selection_handle_location(view, raycast.val.location);
			update_world_from_location_and_input(corner_pos, buttons_down, raycast.val.location, glms_vec3_add(raycast.val.world_block_position, raycast.val.box_raycast.normal));
		}
		character_apply_physics(corner_pos, frame_delta);
		character_apply_velocity(frame_delta);
		game::update_camera(cam, now);

		game::update_needed(view, perspective_3d, cam);

		if (cam.update_view) {
			skybox_update(view, cam.position);
			block_selection_update(view);
		}
		debug_ui.update(buttons_down);

		GX_LoadProjectionMtx(perspective_3d, GX_PERSPECTIVE);
		skybox_draw();

		GX_SetCurrentMtx(BLOCK_WORLD_MATRIX_INDEX);

		draw_block_display_lists(view);
		
		if (raycast.success) {
			block_selection_draw(now);
		}
		
		GX_LoadProjectionMtx(perspective_2d, GX_ORTHOGRAPHIC);
		game::init_ui_rendering();
		water_overlay_draw();
		cursor_draw();

		game::init_text_rendering();
		debug_ui.draw({ character_position.raw[0], character_position.raw[1], character_position.raw[2] }, { cam.look.raw[0], cam.look.raw[1], cam.look.raw[2] }, total_block_gen_time, total_mesh_gen_time, last_mesh_gen_time, std::ceil(fps));

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
