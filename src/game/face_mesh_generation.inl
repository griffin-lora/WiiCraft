#pragma once
#include "face_mesh_generation.hpp"

ADD_FACE_TEMPLATE void game::add_flat_standard_front_vertices(SHORT_FACE_PARAMS) {
    // +x
    ms_st.template add_quad<L>(standard_quad{
        { { po.x,po.y, p.z }, { u.x,u.y } },
        { { po.x,p.y, p.z }, { u.x, uo.y } },
        { { po.x,p.y,po.z }, { uo.x,uo.y } },
        { { po.x,po.y,po.z }, { uo.x, u.y } }
    });
}

ADD_FACE_TEMPLATE void game::add_flat_standard_back_vertices(SHORT_FACE_PARAMS) {
    // -x
    ms_st.template add_quad<L>(standard_quad{
        { {  p.x, po.y, p.z }, { u.x,u.y } },	// Top Left of the quad (top)
        { { p.x, po.y, po.z }, { uo.x,u.y } },	// Top Right of the quad (top)
        { { p.x, p.y, po.z }, { uo.x,uo.y } },	// Bottom Right of the quad (top)
        { { p.x, p.y, p.z }, { u.x,uo.y } }		// Bottom Left of the quad (top)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_standard_top_vertices(SHORT_FACE_PARAMS) {
    // +y
    ms_st.template add_quad<L>(standard_quad{
        { { p.x,po.y,po.z }, { u.x,u.y } },	// Bottom Left Of The Quad (Back)
        { { p.x,po.y,p.z }, { uo.x,u.y } },	// Bottom Right Of The Quad (Back)
        { { po.x, po.y,p.z }, { uo.x,uo.y } },	// Top Right Of The Quad (Back)
        { { po.x, po.y,po.z }, { u.x,uo.y } }	// Top Left Of The Quad (Back)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_standard_bottom_vertices(SHORT_FACE_PARAMS) {
    // -y
    ms_st.template add_quad<L>(standard_quad{
        { { p.x, p.y, po.z }, { u.x, u.y } },		// Top Right Of The Quad (Front)
        { { po.x, p.y, po.z }, { uo.x, u.y } },	// Top Left Of The Quad (Front)
        { { po.x, p.y, p.z }, { uo.x, uo.y } },	// Bottom Left Of The Quad (Front)
        { { p.x, p.y, p.z }, { u.x, uo.y } }	// Bottom Right Of The Quad (Front)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_standard_right_vertices(SHORT_FACE_PARAMS) {
    // +z
    ms_st.template add_quad<L>(standard_quad{
        { { po.x, p.y,po.z }, { u.x,uo.y } },	// Top Right Of The Quad (Right)
        { { p.x, p.y, po.z }, { uo.x,uo.y } },		// Top Left Of The Quad (Right)
        { { p.x,po.y, po.z }, { uo.x,u.y } },	// Bottom Left Of The Quad (Right)
        { { po.x,po.y,po.z }, { u.x,u.y } }	// Bottom Right Of The Quad (Right)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_standard_left_vertices(SHORT_FACE_PARAMS) {
    // -z
    ms_st.template add_quad<L>(standard_quad{
        { { po.x, p.y, p.z }, { u.x,uo.y } },	// Top Right Of The Quad (Left)
        { { po.x, po.y,p.z }, { u.x,u.y } },	// Top Left Of The Quad (Left)
        { { p.x,po.y,p.z }, { uo.x,u.y } },	// Bottom Left Of The Quad (Left)
        { { p.x,p.y, p.z }, { uo.x,uo.y } }	// Bottom Right Of The Quad (Left)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_front_vertices(SHORT_TINTED_FACE_PARAMS) {
    // +x
    ms_st.template add_quad<L>(tinted_quad{
        { { po.x,po.y, p.z }, c, { u.x,u.y } },
        { { po.x,p.y, p.z }, c, { u.x, uo.y } },
        { { po.x,p.y,po.z }, c, { uo.x,uo.y } },
        { { po.x,po.y,po.z }, c, { uo.x, u.y } }
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_back_vertices(SHORT_TINTED_FACE_PARAMS) {
    // -x
    ms_st.template add_quad<L>(tinted_quad{
        { {  p.x, po.y, p.z }, c, { u.x,u.y } },	// Top Left of the quad (top)
        { { p.x, po.y, po.z }, c, { uo.x,u.y } },	// Top Right of the quad (top)
        { { p.x, p.y, po.z }, c, { uo.x,uo.y } },	// Bottom Right of the quad (top)
        { { p.x, p.y, p.z }, c, { u.x,uo.y } }		// Bottom Left of the quad (top)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_top_vertices(SHORT_TINTED_FACE_PARAMS) {
    // +y
    ms_st.template add_quad<L>(tinted_quad{
        { { p.x,po.y,po.z }, c, { u.x,u.y } },	// Bottom Left Of The Quad (Back)
        { { p.x,po.y,p.z }, c, { uo.x,u.y } },	// Bottom Right Of The Quad (Back)
        { { po.x, po.y,p.z }, c, { uo.x,uo.y } },	// Top Right Of The Quad (Back)
        { { po.x, po.y,po.z }, c, { u.x,uo.y } }	// Top Left Of The Quad (Back)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_bottom_vertices(SHORT_TINTED_FACE_PARAMS) {
    // -y
    ms_st.template add_quad<L>(tinted_quad{
        { { p.x, p.y, po.z }, c, { u.x, u.y } },		// Top Right Of The Quad (Front)
        { { po.x, p.y, po.z }, c, { uo.x, u.y } },	// Top Left Of The Quad (Front)
        { { po.x, p.y, p.z }, c, { uo.x, uo.y } },	// Bottom Left Of The Quad (Front)
        { { p.x, p.y, p.z }, c, { u.x, uo.y } }	// Bottom Right Of The Quad (Front)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_right_vertices(SHORT_TINTED_FACE_PARAMS) {
    // +z
    ms_st.template add_quad<L>(tinted_quad{
        { { po.x, p.y,po.z }, c, { u.x,uo.y } },	// Top Right Of The Quad (Right)
        { { p.x, p.y, po.z }, c, { uo.x,uo.y } },		// Top Left Of The Quad (Right)
        { { p.x,po.y, po.z }, c, { uo.x,u.y } },	// Bottom Left Of The Quad (Right)
        { { po.x,po.y,po.z }, c, { u.x,u.y } }	// Bottom Right Of The Quad (Right)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_left_vertices(SHORT_TINTED_FACE_PARAMS) {
    // -z
    ms_st.template add_quad<L>(tinted_quad{
        { { po.x, p.y, p.z }, c, { u.x,uo.y } },	// Top Right Of The Quad (Left)
        { { po.x, po.y,p.z }, c, { u.x,u.y } },	// Top Left Of The Quad (Left)
        { { p.x,po.y,p.z }, c, { uo.x,u.y } },	// Bottom Left Of The Quad (Left)
        { { p.x,p.y, p.z }, c, { uo.x,uo.y } }	// Bottom Right Of The Quad (Left)
    });
}


ADD_FACE_TEMPLATE void game::add_flat_tinted_decal_front_vertices(SHORT_TINTED_DECAL_FACE_PARAMS) {
    // +x
    ms_st.template add_quad<L>(tinted_decal_quad{
        { { po.x,po.y, p.z }, c, { u0.x, u0.y }, { u1.x, u1.y } },
        { { po.x,p.y, p.z }, c, { u0.x, uo0.y }, { u1.x, uo1.y } },
        { { po.x,p.y,po.z }, c, { uo0.x,uo0.y }, { uo1.x,uo1.y } },
        { { po.x,po.y,po.z }, c, { uo0.x, u0.y }, { uo1.x, u1.y } }
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_decal_back_vertices(SHORT_TINTED_DECAL_FACE_PARAMS) {
    // -x
    ms_st.template add_quad<L>(tinted_decal_quad{
        { {  p.x, po.y, p.z }, c, { u0.x,u0.y }, { u1.x,u1.y } },	// Top Left of the quad (top)
        { { p.x, po.y, po.z }, c, { uo0.x,u0.y }, { uo1.x,u1.y } },	// Top Right of the quad (top)
        { { p.x, p.y, po.z }, c, { uo0.x,uo0.y }, { uo1.x,uo1.y } },	// Bottom Right of the quad (top)
        { { p.x, p.y, p.z }, c, { u0.x,uo0.y }, { u1.x,uo1.y } }		// Bottom Left of the quad (top)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_decal_top_vertices(SHORT_TINTED_DECAL_FACE_PARAMS) {
    // +y
    ms_st.template add_quad<L>(tinted_decal_quad{
        { { p.x,po.y,po.z }, c, { u0.x,u0.y }, { u1.x,u1.y } },	// Bottom Left Of The Quad (Back)
        { { p.x,po.y,p.z }, c, { uo0.x,u0.y }, { uo1.x,u1.y } },	// Bottom Right Of The Quad (Back)
        { { po.x, po.y,p.z }, c, { uo0.x,uo0.y }, { uo1.x,uo1.y } },	// Top Right Of The Quad (Back)
        { { po.x, po.y,po.z }, c, { u0.x,uo0.y }, { u1.x,uo1.y } }	// Top Left Of The Quad (Back)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_decal_bottom_vertices(SHORT_TINTED_DECAL_FACE_PARAMS) {
    // -y
    ms_st.template add_quad<L>(tinted_decal_quad{
        { { p.x, p.y, po.z }, c, { u0.x, u0.y }, { u1.x, u1.y } },		// Top Right Of The Quad (Front)
        { { po.x, p.y, po.z }, c, { uo0.x, u0.y }, { uo1.x, u1.y } },	// Top Left Of The Quad (Front)
        { { po.x, p.y, p.z }, c, { uo0.x, uo0.y }, { uo1.x, uo1.y } },	// Bottom Left Of The Quad (Front)
        { { p.x, p.y, p.z }, c, { u0.x, uo0.y }, { u1.x, uo1.y } }	// Bottom Right Of The Quad (Front)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_decal_right_vertices(SHORT_TINTED_DECAL_FACE_PARAMS) {
    // +z
    ms_st.template add_quad<L>(tinted_decal_quad{
        { { po.x, p.y,po.z }, c, { u0.x,uo0.y }, { u1.x,uo1.y } },	// Top Right Of The Quad (Right)
        { { p.x, p.y, po.z }, c, { uo0.x,uo0.y }, { uo1.x,uo1.y } },		// Top Left Of The Quad (Right)
        { { p.x,po.y, po.z }, c, { uo0.x,u0.y }, { uo1.x,u1.y } },	// Bottom Left Of The Quad (Right)
        { { po.x,po.y,po.z }, c, { u0.x,u0.y }, { u1.x,u1.y } }	// Bottom Right Of The Quad (Right)
    });
}

ADD_FACE_TEMPLATE void game::add_flat_tinted_decal_left_vertices(SHORT_TINTED_DECAL_FACE_PARAMS) {
    // -z
    ms_st.template add_quad<L>(tinted_decal_quad{
        { { po.x, p.y, p.z }, c, { u0.x,uo0.y }, { u1.x,uo1.y } },	// Top Right Of The Quad (Left)
        { { po.x, po.y,p.z }, c, { u0.x,u0.y }, { u1.x,u1.y } },	// Top Left Of The Quad (Left)
        { { p.x,po.y,p.z }, c, { uo0.x,u0.y }, { uo1.x,u1.y } },	// Bottom Left Of The Quad (Left)
        { { p.x,p.y, p.z }, c, { uo0.x,uo0.y }, { uo1.x,uo1.y } }	// Bottom Right Of The Quad (Left)
    });
}

ADD_FACE_TEMPLATE void game::add_foliage_vertices(SHORT_TINTED_FACE_PARAMS) {
    ms_st.template add_quad<L>(tinted_quad{
        { { p.x, p.y, p.z }, c, { u.x, uo.y } },
        { { po.x, p.y, po.z }, c, { uo.x, uo.y } },
        { { po.x, po.y, po.z }, c, { uo.x, u.y } },
        { { p.x, po.y, p.z }, c, { u.x, u.y } },
    });

    ms_st.template add_quad<L>(tinted_quad{
        { { po.x, p.y, p.z }, c, { u.x, uo.y } },
        { { p.x, p.y, po.z }, c, { uo.x, uo.y } },
        { { p.x, po.y, po.z }, c, { uo.x, u.y } },
        { { po.x, po.y, p.z }, c, { u.x, u.y } },
    });
}