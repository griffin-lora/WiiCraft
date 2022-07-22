#pragma once
#include <cstdint>

namespace game {
    struct block_traits {
        bool visible = false;
    };

    struct face_traits {
        bool visible = false;
        bool partially_transparent = true;
        bool bottom_half_transparent = true;
        bool top_half_transparent = true;
    };

    enum class block_counting_type {
        INVISIBLE,
        FULLY_OPAQUE,
        PARTIALLY_OPAQUE
    };
};