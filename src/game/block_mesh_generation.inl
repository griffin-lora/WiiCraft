#include "block_mesh_generation.hpp"

template<typename F1, typename F2>
void game::write_into_display_lists(const block_quad_iterators& begin, const block_mesh_state& vf,
    gfx::display_list& standard_disp_list,
    gfx::display_list& foliage_disp_list,
    gfx::display_list& water_disp_list,
    F1 get_display_list_size, F2 write_vert
) {
    using const_quad_it = ext::data_array<chunk::quad>::const_iterator;

    auto write_into_disp_list = [&get_display_list_size, &write_vert](const_quad_it begin, const_quad_it end, gfx::display_list& disp_list) {
        std::size_t vert_count = (end - begin) * 4;

        disp_list.resize(get_display_list_size(vert_count));

        disp_list.write_into([&begin, &end, &write_vert, vert_count]() {
            GX_Begin(GX_QUADS, GX_VTXFMT0, vert_count);

            for (auto it = begin; it != end; ++it) {
                write_vert(it->vert0);
                write_vert(it->vert1);
                write_vert(it->vert2);
                write_vert(it->vert3);
            }
            
            GX_End();
        });
    };

    write_into_disp_list(begin.standard, vf.it.standard, standard_disp_list);
    write_into_disp_list(begin.foliage, vf.it.foliage, foliage_disp_list);
    write_into_disp_list(begin.water, vf.it.water, water_disp_list);
}