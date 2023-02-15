#include "game_math.h"
#include "util.h"
#include <math.h>

static f32 get_noise_at_grid_position(s32vec2s pos) {
    s32 val = mod_s32((pos.x * 374761393) + (pos.y * 668265263), 1274126177);
    return val / 1274126177.0f;
}

f32 get_eased(f32 x) {
    return (
        6 * (x * x * x * x * x) -
        15 * (x * x * x * x) +
        10 * (x * x * x)
    );
}

static f32 lerpf(f32 min, f32 max, f32 alpha) {
    return min + alpha * (max - min);
}

f32 get_noise_at(vec2s pos) {
    s32vec2s floor_pos = {
        (s32)floorf(pos.x),
        (s32)floorf(pos.y)
    };
    s32vec2s ceil_pos = {
        (s32)ceilf(pos.x),
        (s32)ceilf(pos.y)
    };

    f32 floor_floor_noise = get_noise_at_grid_position(floor_pos);
    f32 ceil_floor_noise = get_noise_at_grid_position((s32vec2s){
        ceil_pos.x,
        floor_pos.y
    });

    f32 floor_ceil_noise = get_noise_at_grid_position((s32vec2s){
        floor_pos.x,
        ceil_pos.y
    });
    f32 ceil_ceil_noise = get_noise_at_grid_position(ceil_pos);

    vec2s dist = {{ pos.x - floor_pos.x, pos.y - floor_pos.y }};

    vec2s eased_dist = {{ get_eased(dist.x), get_eased(dist.y) }};

    f32 x_val_a = lerpf(floor_floor_noise, ceil_floor_noise, eased_dist.x);
    f32 x_val_b = lerpf(floor_ceil_noise, ceil_ceil_noise, eased_dist.x);

    return lerpf(x_val_a, x_val_b, eased_dist.y);
}