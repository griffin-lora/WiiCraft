#pragma once
#include "chunk.hpp"

namespace game {
    void update_mesh(chunk& chunk, ext::data_array<chunk::quad>& building_quads);
}