#pragma once
#include "chunk.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list.hpp"

namespace game {
    struct block_quad_building_arrays {
        static constexpr std::size_t SAFE_BUFFER_OVERFLOW_SIZE = 0x100;

        using quad_array = ext::data_array<chunk::quad>;
        quad_array standard;
        quad_array foliage;
        quad_array water;

        inline block_quad_building_arrays() : standard(chunk::MAX_STANDARD_QUAD_COUNT + SAFE_BUFFER_OVERFLOW_SIZE), foliage(chunk::MAX_FOLIAGE_QUAD_COUNT + SAFE_BUFFER_OVERFLOW_SIZE), water(chunk::MAX_WATER_QUAD_COUNT + SAFE_BUFFER_OVERFLOW_SIZE) {}
    };
    
    struct block_quad_iterators {
        using quad_it = ext::data_array<chunk::quad>::iterator;
        quad_it standard;
        quad_it foliage;
        quad_it water;

        block_quad_iterators(block_quad_building_arrays& arrays) : standard(arrays.standard.begin()), foliage(arrays.foliage.begin()), water(arrays.water.begin()) {}
    };

    struct block_mesh_state {
        block_quad_iterators it;

        inline void add_standard(const chunk::quad& quad) {
            *it.standard++ = quad;
        }

        inline void add_foliage(const chunk::quad& quad) {
            *it.foliage++ = quad;
        }

        inline void add_water(const chunk::quad& quad) {
            *it.water++ = quad;
        }
    };

    template<typename F1, typename F2>
    void write_into_display_lists(const block_quad_iterators& begin, const block_quad_iterators& end,
        gfx::display_list& standard_disp_list,
        gfx::display_list& foliage_disp_list,
        gfx::display_list& water_disp_list,
        F1 get_display_list_size, F2 write_vert
    );
}