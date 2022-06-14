#pragma once
#include "camera.hpp"
#include "chunk.hpp"

namespace game {
    void destroy_block_from_camera(const camera& cam, chunk::map& chunks);
};