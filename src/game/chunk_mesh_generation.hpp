#pragma once
#include "chunk.hpp"

namespace game {
    void update_mesh(chunk& chunk, chunk::mesh& mesh, ext::data_array<game::block::face_cache>& face_caches);
};