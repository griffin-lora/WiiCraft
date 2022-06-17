#pragma once
#include "chunk.hpp"
#include "camera.hpp"
#include "gfx.hpp"

namespace game {
    void init_chunk_drawing();
    void draw_chunk(chunk& chunk);

    void draw_chunks(gfx::texture& chunk_tex, math::matrix view, const camera& cam, chunk::map& chunks);
};