#pragma once
#include <gctypes.h>

// Not std::chrono since that doesn't work :)
namespace chrono {
    // Microseconds
    using us = u32;

    template<typename T>
    using us_tp = T;

    us_tp<s64> get_current_us();
};