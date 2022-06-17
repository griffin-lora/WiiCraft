#pragma once
#include "camera.hpp"
#include "chunk.hpp"
#include "block.hpp"
#include <optional>
#include <tuple>

namespace game {
    struct raycast {
        math::vector3u8 position;
        chunk& ch;
        block& bl;
    };

    std::optional<raycast> get_raycast(const camera& cam, chunk::map& chunks);
};