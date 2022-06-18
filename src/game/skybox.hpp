#pragma once
#include "math/transform_3d.hpp"
#include "gfx/display_list.hpp"
#include "gfx.hpp"
#include "camera.hpp"

namespace game {
    struct skybox {
        math::transform_3d tf;
        gfx::display_list disp_list;

        skybox(const math::matrix view, const camera& cam);
        void update(const math::matrix view, const camera& cam);
        void update_if_needed(const math::matrix view, const camera& cam);
        void draw(const gfx::texture& texture) const;
    };
}