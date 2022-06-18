#pragma once
#include "math/transform_3d.hpp"
#include "gfx/display_list.hpp"
#include "gfx.hpp"
#include "camera.hpp"

namespace game {
    struct skybox {
        math::transform_3d tf;
        gfx::display_list disp_list;

        skybox(math::matrix view, const camera& cam);
        void update(math::matrix view, const camera& cam);
        void update_if_needed(math::matrix view, const camera& cam);
        void draw(gfx::texture& texture);
    };
}