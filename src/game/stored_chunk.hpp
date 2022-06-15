#pragma once
#include "block.hpp"
#include "ext/data_array.hpp"

namespace game {
    struct stored_chunk {
        ext::data_array<block> blocks;
    };
};