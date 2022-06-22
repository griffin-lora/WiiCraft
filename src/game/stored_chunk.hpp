#pragma once
#include "block.hpp"
#include "ext/data_array.hpp"
#include "common.hpp"

namespace game {
    struct stored_chunk {
        using map = std::unordered_map<math::vector3s32, stored_chunk, hash_vector3s32>;

        ext::data_array<block> blocks;
    };
};