#pragma once
#include "math.hpp"
#include "camera.hpp"
#include "chunk.hpp"

namespace game {
    struct character {
        glm::vec3 position;
        glm::vec3 velocity;

        void handle_input(f32 movement_speed, const camera& cam, u32 buttons_down);
        void move(f32 movement_speed, const camera& cam, const glm::vec3& input_vector);

        void apply_physics(f32 gravity, chunk::map& chunks);

        void update_camera(camera& cam) const;
    };
}