#pragma once
#include "chunk.hpp"

namespace game {
    void update_mesh(chunk& chunk, ext::data_array<game::block::face_cache>& face_caches);
};