#pragma once
#include <cstdint>

namespace game {
    struct block_traits {
        bool visible = false;
        std::size_t general_vertex_count = 0;
    };

    struct face_traits {
        bool partially_transparent = true;
        std::size_t vertex_count = 0;
    };
};