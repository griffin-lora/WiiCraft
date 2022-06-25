#pragma once
#include <cstdint>

namespace game {
    struct block_traits {
        bool visible = false;
        bool general_visible = false;
    };

    struct face_traits {
        bool visible = false;
        bool partially_transparent = true;
    };
};