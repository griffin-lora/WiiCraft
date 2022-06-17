#pragma once
#include "chunk.hpp"

namespace game {
    struct chunk_mesh_vert_func {
        static inline void call(u8 x, u8 y, u8 z, u8 u, u8 v) {
            GX_Position3u8(x, y, z);
            GX_TexCoord2u8(u, v);
        }
    };

    void update_mesh(chunk& chunk, ext::data_array<game::block::face_cache>& face_caches);
};