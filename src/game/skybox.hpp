#pragma once
#include "math/transform_3d.hpp"
#include "gfx/display_list.hpp"
#include "gfx.hpp"
#include "camera.hpp"

namespace game {
    struct skybox {
        math::transform_3d tf;
        gfx::display_list disp_list;
    };

    void init(skybox& skybox);

    void update_skybox(math::matrix view, const camera& cam, skybox& skybox);

    void draw_skybox(gfx::texture& texture, skybox& skybox);
}