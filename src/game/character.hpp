#pragma once
#include "math.hpp"
#include "camera.hpp"
#include "chunk.hpp"

namespace game {
    struct character {
        glm::vec3 position;
        glm::vec3 velocity;

        bool grounded = false;

        void handle_input(const camera& cam, f32 delta, u32 buttons_down);

        void apply_movement(const camera& cam, f32 delta, glm::vec3 input_vector);
        void apply_no_movement(f32 delta);
        
        void apply_physics(chunk::map& chunks, f32 delta);
        void apply_velocity(f32 delta);

        void update_camera(camera& cam) const;

        private:
            bool apply_collision(chunk::map& chunks, f32 delta);
    };
}