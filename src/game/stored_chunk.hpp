#pragma once
#include "block.hpp"
#include "ext/data_array.hpp"

namespace game {
    struct stored_chunk {
        using map = std::unordered_map<math::vector3s32, stored_chunk>;

        ext::data_array<block> blocks;
    };
};