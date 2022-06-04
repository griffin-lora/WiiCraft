#pragma once

namespace game {
    enum class face {
        center,
        front,
        back,
        left,
        right,
        top,
        bottom
    };

    struct block {
        enum class type {
            AIR,
            GRASS
        };
        type tp;
    };
}