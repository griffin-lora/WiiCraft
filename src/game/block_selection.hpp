#pragma once
#include "block_raycast.hpp"
#include "logic.hpp"
#include "math.hpp"
#include "block_mesh_generation.hpp"
#include "chunk_mesh_generation.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_3d.hpp"
#include <optional>

inline void block_selection_init() {}
void block_selection_update(const Mtx view);
void block_selection_draw(chrono::us now);
void block_selection_handle_raycast(const Mtx view, decltype(game::chunk_quad_building_arrays::standard)& building_array, const block_raycast_wrap_t& raycast);