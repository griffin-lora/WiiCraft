#include "character.h"
#include "block_raycast.h"
#include "input.h"
#include <math.h>

#define MOVEMENT_ACCEL 40.0f
#define MAX_WALKING_SPEED 6.0f
#define MAX_SPRINTING_SPEED 9.0f
#define MOVEMENT_DECEL_FACTOR 0.005f
#define GRAVITY 36.0f
#define JUMP_VELOCITY 10.0f

#define SHAKING_ACCEL 100
#define NUNCHUK_SHAKING_ACCEL 100

vec3s character_position;
vec3s character_velocity;

bool grounded = false;
bool sprinting = false;

us_t fov_tween_start = 0;



static void apply_movement(vec3s cam_look, us_t now, f32 delta, bool shaking, vec3s input_vector) {
    if (shaking && !sprinting) {
        sprinting = true;
        fov_tween_start = now;
    }

    mat3s movement_matrix = {{
        { cam_look.x, 0, cam_look.z },
        { 0, 1, 0 },
        { -cam_look.z, 0, cam_look.x }
    }};
    glm_vec3_normalize(movement_matrix.col[0].raw);
    glm_vec3_normalize(movement_matrix.col[1].raw);
    glm_vec3_normalize(movement_matrix.col[2].raw);

    input_vector = glms_mat3_mulv(movement_matrix, input_vector);

    vec3s accel_vector = glms_vec3_scale(input_vector, MOVEMENT_ACCEL);

    vec3s move_vector = { .x = character_velocity.x, .y = 0.0f, .z = character_velocity.z };
    move_vector = glms_vec3_add(move_vector, glms_vec3_scale(accel_vector, delta));

    if (IS_NON_ZERO_VEC3(move_vector) && IS_NON_ZERO_VEC3(input_vector)) {
        f32 len = glms_vec3_norm(move_vector);
        f32 max = (sprinting ? MAX_SPRINTING_SPEED : MAX_WALKING_SPEED) * glms_vec3_norm(input_vector);
        if (len > max) {
            move_vector = glms_vec3_divs(move_vector, len);
            move_vector = glms_vec3_scale(move_vector, max);
        }
    }
    character_velocity = (vec3s){ .x = move_vector.x, .y = character_velocity.y, .z = move_vector.z };
}

static void apply_no_movement(us_t now, f32 delta) {
    if (sprinting) {
        sprinting = false;
        fov_tween_start = now;
    }

    vec3s move_vector = { .x = character_velocity.x, .y = 0.0f, .z = character_velocity.z };
    if (IS_NON_ZERO_VEC3(move_vector)) {
        move_vector = glms_vec3_scale(move_vector, MOVEMENT_DECEL_FACTOR * delta);
        character_velocity = (vec3s){ .x = move_vector.x, .y = character_velocity.y, .z = move_vector.z };
    }
}

static const vec3s half_size = { .x = 0.35f, .y = 0.9f, .z = 0.35f };

static bool apply_collision(s32vec3s corner_pos, f32 delta) {
    vec3s direction = glms_vec3_scale(character_velocity, delta);

    vec3s begin = glms_vec3_sub(character_position, half_size);
    vec3s end = glms_vec3_add(character_position, half_size);

    vec3s next_character_position = glms_vec3_add(character_position, direction);

    vec3s next_begin = glms_vec3_sub(next_character_position, half_size);
    vec3s next_end = glms_vec3_add(next_character_position, half_size);

    if (next_begin.x < begin.x) {
        begin.x = next_begin.x;
    }
    if (next_begin.y < begin.y) {
        begin.y = next_begin.y;
    }
    if (next_begin.z < begin.z) {
        begin.z = next_begin.z;
    }
    if (next_end.x > end.x) {
        end.x = next_end.x;
    }
    if (next_end.y > end.y) {
        end.y = next_end.y;
    }
    if (next_end.z > end.z) {
        end.z = next_end.z;
    }

    block_raycast_wrap_t raycast = get_block_raycast(corner_pos, character_position, direction, begin, end, half_size, block_box_type_collision);

    if (raycast.success) {
        vec3s normal = raycast.val.box_raycast.normal;
        if (IS_NON_ZERO_VEC3(normal)) {
            if (normal.y == 1.0f) {
                grounded = true;
            }
            vec3s absolute_normal = glms_vec3_abs(normal);
            vec3s inverse_normal = { .x = absolute_normal.x != 0 ? 0 : 1, .y = absolute_normal.y != 0 ? 0 : 1, .z = absolute_normal.z != 0 ? 0 : 1 };

            character_velocity = glms_vec3_mul(character_velocity, inverse_normal);
            return true;
        }
    }
    return false;
}


void character_handle_input(vec3s cam_look, vec3w_t last_wpad_accel, vec3w_t last_nunchuk_accel, us_t now, f32 delta, vec3w_t wpad_accel, vec2s joystick_input_vector, u8 nunchuk_buttons_down, vec3w_t nunchuk_accel) {
    if ((nunchuk_buttons_down & NUNCHUK_BUTTON_C) && grounded) {
        character_velocity.y = JUMP_VELOCITY;
    }

    vec3w_t wpad_diff = {
        wpad_accel.x - last_wpad_accel.x,
        wpad_accel.y - last_wpad_accel.y,
        wpad_accel.z - last_wpad_accel.z
    };
    vec3w_t nunchuk_diff = {
        nunchuk_accel.x - last_nunchuk_accel.x,
        nunchuk_accel.y - last_nunchuk_accel.y,
        nunchuk_accel.z - last_nunchuk_accel.z
    };
    bool shaking = LENGTH_SQUARED_VEC3(wpad_diff) > (SHAKING_ACCEL * SHAKING_ACCEL) || LENGTH_SQUARED_VEC3(nunchuk_diff) > (NUNCHUK_SHAKING_ACCEL * NUNCHUK_SHAKING_ACCEL);

    if (IS_NON_ZERO_VEC2(joystick_input_vector)) {
        if (fabsf(joystick_input_vector.x) < 6.0f) {
            joystick_input_vector.x = 0.0f;
        }
        if (fabsf(joystick_input_vector.y) < 6.0f) {
            joystick_input_vector.y = 0.0f;
        }
        if (joystick_input_vector.x == 0.0f && joystick_input_vector.y == 0.0f) {
            apply_no_movement(now, delta);
        } else {
            vec3s input_vector = { .x = joystick_input_vector.y / 96.0f, .y = 0.0f, .z = joystick_input_vector.x / 96.0f };
            apply_movement(cam_look, now, delta, shaking, input_vector);
        }
    } else {
        apply_no_movement(now, delta);
    }
}

void character_apply_physics(s32vec3s corner_pos, f32 delta) {
    #ifndef PC_PORT
	vec3s div_pos = glms_vec3_scale(character_position, 1.0f/(f32)NUM_ROW_BLOCKS_PER_BLOCK_CHUNK);
    s32vec3s chunk_rel_pos = {
        .x = (s32)floorf(div_pos.x),
        .y = (s32)floorf(div_pos.y),
        .z = (s32)floorf(div_pos.z)
    };
    glm_ivec3_sub(chunk_rel_pos.raw, corner_pos.raw, chunk_rel_pos.raw);

    size_t index = (chunk_rel_pos.z * BLOCK_POOL_CHUNK_INDICES_Z_OFFSET) + (chunk_rel_pos.y * BLOCK_POOL_CHUNK_INDICES_Y_OFFSET) + (chunk_rel_pos.x * BLOCK_POOL_CHUNK_INDICES_X_OFFSET);

    if (index >= NUM_BLOCK_CHUNKS) {
        character_velocity.y = 0;
        return;
    }

    if (!(block_pool_chunk_bitfields[index] & BLOCK_CHUNK_FLAG_HAS_VALID_BLOCKS)) {
        character_velocity.y = 0;
        return;
    }

    character_velocity.y -= GRAVITY * delta;

    grounded = false;

    while (apply_collision(corner_pos, delta));
    #endif
}

void character_apply_velocity(f32 delta) {
    character_position = glms_vec3_add(character_position, glms_vec3_scale(character_velocity, delta));
}