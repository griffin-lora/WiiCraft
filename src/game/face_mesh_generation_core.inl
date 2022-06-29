#pragma once
#include "face_mesh_generation_core.hpp"

template<typename Vf>
void game::add_flat_front_vertices(Vf& vf, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
    auto l = d_positions.block_draw_pos;
    auto lo = offset_d_positions.block_draw_pos;
    auto u = d_positions.uv_draw_pos;
    auto uo = offset_d_positions.uv_draw_pos;

    // +x
    vf.add_standard(chunk::quad::vertices{
        { { lo.x,lo.y, l.z }, { u.x,u.y } },
        { { lo.x,l.y, l.z }, { u.x, uo.y } },
        { { lo.x,l.y,lo.z }, { uo.x,uo.y } },
        { { lo.x,lo.y,lo.z }, { uo.x, u.y } }
    });
}

template<typename Vf>
void game::add_flat_back_vertices(Vf& vf, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
    auto l = d_positions.block_draw_pos;
    auto lo = offset_d_positions.block_draw_pos;
    auto u = d_positions.uv_draw_pos;
    auto uo = offset_d_positions.uv_draw_pos;

    // -x
    vf.add_standard(chunk::quad::vertices{
        { {  l.x, lo.y, l.z }, { u.x,u.y } },	// Top Left of the quad (top)
        { { l.x, lo.y, lo.z }, { uo.x,u.y } },	// Top Right of the quad (top)
        { { l.x, l.y, lo.z }, { uo.x,uo.y } },	// Bottom Right of the quad (top)
        { { l.x, l.y, l.z }, { u.x,uo.y } }		// Bottom Left of the quad (top)
    });
}

template<typename Vf>
void game::add_flat_top_vertices(Vf& vf, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
    auto l = d_positions.block_draw_pos;
    auto lo = offset_d_positions.block_draw_pos;
    auto u = d_positions.uv_draw_pos;
    auto uo = offset_d_positions.uv_draw_pos;
    
    // +y

    vf.add_standard(chunk::quad::vertices{
        { { l.x,lo.y,lo.z }, { u.x,u.y } },	// Bottom Left Of The Quad (Back)
        { { l.x,lo.y,l.z }, { uo.x,u.y } },	// Bottom Right Of The Quad (Back)
        { { lo.x, lo.y,l.z }, { uo.x,uo.y } },	// Top Right Of The Quad (Back)
        { { lo.x, lo.y,lo.z }, { u.x,uo.y } }	// Top Left Of The Quad (Back)
    });
}

template<typename Vf>
void game::add_flat_bottom_vertices(Vf& vf, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
    auto l = d_positions.block_draw_pos;
    auto lo = offset_d_positions.block_draw_pos;
    auto u = d_positions.uv_draw_pos;
    auto uo = offset_d_positions.uv_draw_pos;

    // -y

    vf.add_standard(chunk::quad::vertices{
        { { l.x, l.y, lo.z }, { u.x, u.y } },		// Top Right Of The Quad (Front)
        { { lo.x, l.y, lo.z }, { uo.x, u.y } },	// Top Left Of The Quad (Front)
        { { lo.x, l.y, l.z }, { uo.x, uo.y } },	// Bottom Left Of The Quad (Front)
        { { l.x, l.y, l.z }, { u.x, uo.y } }	// Bottom Right Of The Quad (Front)
    });
}

template<typename Vf>
void game::add_flat_right_vertices(Vf& vf, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
    auto l = d_positions.block_draw_pos;
    auto lo = offset_d_positions.block_draw_pos;
    auto u = d_positions.uv_draw_pos;
    auto uo = offset_d_positions.uv_draw_pos;

    // +z

    vf.add_standard(chunk::quad::vertices{
        { { lo.x, l.y,lo.z }, { u.x,uo.y } },	// Top Right Of The Quad (Right)
        { { l.x, l.y, lo.z }, { uo.x,uo.y } },		// Top Left Of The Quad (Right)
        { { l.x,lo.y, lo.z }, { uo.x,u.y } },	// Bottom Left Of The Quad (Right)
        { { lo.x,lo.y,lo.z }, { u.x,u.y } }	// Bottom Right Of The Quad (Right)
    });
}

template<typename Vf>
void game::add_flat_left_vertices(Vf& vf, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
    auto l = d_positions.block_draw_pos;
    auto lo = offset_d_positions.block_draw_pos;
    auto u = d_positions.uv_draw_pos;
    auto uo = offset_d_positions.uv_draw_pos;
    
    // -z

    vf.add_standard(chunk::quad::vertices{
        { { lo.x, l.y, l.z }, { u.x,uo.y } },	// Top Right Of The Quad (Left)
        { { lo.x, lo.y,l.z }, { u.x,u.y } },	// Top Left Of The Quad (Left)
        { { l.x,lo.y,l.z }, { uo.x,u.y } },	// Bottom Left Of The Quad (Left)
        { { l.x,l.y, l.z }, { uo.x,uo.y } }	// Bottom Right Of The Quad (Left)
    });
}

template<typename Vf>
void game::add_foliage_vertices(Vf& vf, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
    auto l = d_positions.block_draw_pos;
    auto lo = offset_d_positions.block_draw_pos;
    auto u = d_positions.uv_draw_pos;
    auto uo = offset_d_positions.uv_draw_pos;

    vf.add_foliage(chunk::quad::vertices{
        { { l.x, l.y, l.z }, { u.x, uo.y } },
        { { lo.x, l.y, lo.z }, { uo.x, uo.y } },
        { { lo.x, lo.y, lo.z }, { uo.x, u.y } },
        { { l.x, lo.y, l.z }, { u.x, u.y } },
    });

    vf.add_foliage(chunk::quad::vertices{
        { { lo.x, l.y, l.z }, { u.x, uo.y } },
        { { l.x, l.y, lo.z }, { uo.x, uo.y } },
        { { l.x, lo.y, lo.z }, { uo.x, u.y } },
        { { lo.x, lo.y, l.z }, { u.x, u.y } },
    });
}