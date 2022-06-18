#pragma once
#include "chunk.hpp"
#include "camera.hpp"
#include "gfx.hpp"

namespace game {
    void init_chunk_drawing();
    void draw_chunk(const chunk& chunk);

    void draw_chunks(const gfx::texture& chunk_tex, const math::matrix view, const camera& cam, chunk::map& chunks);
};