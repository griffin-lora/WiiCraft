#pragma once
#include "chunk.hpp"

namespace game {
    struct chunk_mesh_vertex_functions {
        static inline void add_pos_vert(u8 x, u8 y, u8 z) {
            GX_Position3u8(x, y, z);
        }
        static inline void add_uv_vert(u8 u, u8 v) {
            GX_TexCoord2u8(u, v);
        }
    };

    void update_mesh(chunk& chunk, ext::data_array<game::block::face_cache>& face_caches);
};