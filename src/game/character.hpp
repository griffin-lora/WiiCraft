#pragma once
#include "math.hpp"
#include "camera.hpp"
#include "chunk.hpp"

namespace game {
    struct character {
        glm::vec3 position;
        glm::vec3 velocity;

        bool grounded = false;

        void handle_input(const camera& cam, u32 buttons_down);

        void apply_movement(const camera& cam, glm::vec3 input_vector);
        void apply_no_movement();
        
        void apply_physics(chunk::map& chunks);
        void apply_velocity();

        void update_camera(camera& cam) const;

        private:
            template<typename Fc, typename Fn>
            void apply_collision(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& dir, Fc collision_func, Fn no_collision_func);
    };
}