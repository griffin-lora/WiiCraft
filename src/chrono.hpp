#pragma once
#include <gctypes.h>

// Not std::chrono since that doesn't work :)
namespace chrono {
    // Microseconds
    using us = s64;

    us get_current_us();
};