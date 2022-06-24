#pragma once
#include "chunk.hpp"

namespace game {
    void update_mesh(chunk& chunk, ext::data_array<chunk::vertex>& building_vertices);
}