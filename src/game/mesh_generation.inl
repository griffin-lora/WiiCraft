#include "mesh_generation.hpp"

template<typename F1, typename F2>
void game::write_into_display_lists(const standard_quad_iterators& begin, const standard_vertex_function& vf,
    gfx::display_list& standard_disp_list,
    gfx::display_list& foliage_disp_list,
    gfx::display_list& water_disp_list,
    F1 get_display_list_size, F2 write_vert
) {
    std::size_t standard_vert_count = (vf.it.standard - begin.standard) * 4;
    std::size_t foliage_vert_count = (vf.it.foliage - begin.foliage) * 4;
    std::size_t water_vert_count = (vf.it.water - begin.water) * 4;

    standard_disp_list.resize(get_display_list_size(standard_vert_count));

    standard_disp_list.write_into([&begin, &vf, &write_vert, standard_vert_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, standard_vert_count);

        for (auto it = begin.standard; it != vf.it.standard; ++it) {
            write_vert(it->vert0);
            write_vert(it->vert1);
            write_vert(it->vert2);
            write_vert(it->vert3);
        }
        
        GX_End();
    });

    foliage_disp_list.resize(get_display_list_size(foliage_vert_count));

    foliage_disp_list.write_into([&begin, &vf, &write_vert, foliage_vert_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, foliage_vert_count);

        for (auto it = begin.foliage; it != vf.it.foliage; ++it) {
            write_vert(it->vert0);
            write_vert(it->vert1);
            write_vert(it->vert2);
            write_vert(it->vert3);
        }
        
        GX_End();
    });

    water_disp_list.resize(get_display_list_size(water_vert_count));

    water_disp_list.write_into([&begin, &vf, &write_vert, water_vert_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, water_vert_count);

        for (auto it = begin.water; it != vf.it.water; ++it) {
            write_vert(it->vert0);
            write_vert(it->vert1);
            write_vert(it->vert2);
            write_vert(it->vert3);
        }
        
        GX_End();
    });
}