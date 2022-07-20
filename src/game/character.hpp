#pragma once
#include "math.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "chrono.hpp"

namespace game {
    struct character {
        glm::vec3 position;
        glm::vec3 velocity;
        
        bool grounded = false;
        bool sprinting = false;

        chrono::us fov_tween_start = 0;

        void handle_input(const camera& cam, const math::vector3u16& last_wpad_accel, const math::vector3u16& last_nunchuk_accel, chrono::us now, f32 delta, const math::vector3u16& wpad_accel, glm::vec2 nunchuk_vector, u8 nunchuk_buttons_down, const math::vector3u16& nunchuk_accel);

        void apply_movement(const camera& cam, chrono::us now, f32 delta, bool shaking, glm::vec3 input_vector);
        void apply_no_movement(chrono::us now, f32 delta);
        
        void apply_physics(chunk::map& chunks, f32 delta);
        void apply_velocity(f32 delta);

        void update_camera(camera& cam, chrono::us now) const;

        private:
            bool apply_collision(chunk::map& chunks, f32 delta);
    };
}