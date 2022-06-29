#include "math.hpp"

using namespace math;

glm::mat3 math::from_euler_angles(f32 yaw, f32 pitch, f32 roll) {
    f32 ch = cosf(yaw);
    f32 sh = sinf(yaw);
    f32 ca = cosf(pitch);
    f32 sa = sinf(pitch);
    f32 cb = cosf(roll);
    f32 sb = sinf(roll);

    return {
        { ch * ca, sh*sb - ch*sa*cb, ch*sa*sb + sh*cb },
        { sa, ca*cb, -ca*sb },
        { -sh*ca, sh*sa*cb + ch*sb, -sh*sa*sb + ch*cb }
    };
}

static f32 get_noise_at_grid_position(const math::vector2s32& pos) {
    s32 val = mod((pos.x * 374761393) + (pos.y * 668265263), 1274126177);
    return (f32)val / 1274126177.0f;
}

static f32 get_eased(f32 x) {
    return (
        6 * (x * x * x * x * x) -
        15 * (x * x * x * x) +
        10 * (x * x * x)
    );
}

static f32 lerp(f32 min, f32 max, f32 alpha) {
    return min + alpha * (max - min);
}

f32 math::get_noise_at(const glm::vec2& pos) {
    math::vector2s32 floor_pos = {
        (s32)std::floor(pos.x),
        (s32)std::floor(pos.y)
    };
    math::vector2s32 ceil_pos = {
        (s32)std::ceil(pos.x),
        (s32)std::ceil(pos.y)
    };

    f32 floor_floor_noise = get_noise_at_grid_position(floor_pos);
    f32 ceil_floor_noise = get_noise_at_grid_position({
        ceil_pos.x,
        floor_pos.y
    });

    f32 floor_ceil_noise = get_noise_at_grid_position({
        floor_pos.x,
        ceil_pos.y
    });
    f32 ceil_ceil_noise = get_noise_at_grid_position(ceil_pos);

    glm::vec2 dist = { pos.x - floor_pos.x, pos.y - floor_pos.y };

    glm::vec2 eased_dist = { get_eased(dist.x), get_eased(dist.y) };

    f32 x_val_a = lerp(floor_floor_noise, ceil_floor_noise, eased_dist.x);
    f32 x_val_b = lerp(floor_ceil_noise, ceil_ceil_noise, eased_dist.x);

    return lerp(x_val_a, x_val_b, eased_dist.y);
}