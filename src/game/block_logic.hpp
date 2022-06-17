#pragma once
#include "camera.hpp"
#include "chunk.hpp"
#include "block.hpp"
#include <optional>
#include <tuple>

namespace game {
    struct raycast {
        math::vector3s32 ch_pos;
        chunk& ch;
        math::vector3u8 bl_pos;
        block& bl;
    };

    std::optional<raycast> get_raycast(const camera& cam, chunk::map& chunks);
};