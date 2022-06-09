#pragma once
#include <cstdlib>
#include <gccore.h>
#include "glm/glm.hpp"

namespace math {
    using vector3s = glm::vec<3, s32, glm::defaultp>;
    using vector3u8 = glm::vec<3, u8, glm::defaultp>;
    using vector3s8 = glm::vec<3, s8, glm::defaultp>;
    using vector2u8 = glm::vec<2, u8, glm::defaultp>;
    using matrix = Mtx;
    using matrix44 = Mtx44;

    template<typename T, glm::qualifier Q>
    inline bool is_non_zero(const glm::vec<2, T, Q>& v) {
        return v.x != 0 || v.y != 0;
    }

    template<typename T, glm::qualifier Q>
    inline bool is_non_zero(const glm::vec<3, T, Q>& v) {
        return v.x != 0 || v.y != 0 || v.z != 0;
    }

    inline const glm::vec3& look(const glm::mat3& mat) {
        return mat[0];
    }

    inline const glm::vec3& up(const glm::mat3& mat) {
        return mat[1];
    }

    inline const glm::vec3& right(const glm::mat3& mat) {
        return mat[2];
    }

    inline glm::vec3& look(glm::mat3& mat) {
        return mat[0];
    }

    inline glm::vec3& up(glm::mat3& mat) {
        return mat[1];
    }

    inline glm::vec3& right(glm::mat3& mat) {
        return mat[2];
    }

    template<typename V>
    inline void normalize(V& v) {
        v = glm::normalize(v);
    }

    glm::mat3 from_euler_angles(f32 yaw, f32 pitch, f32 roll);
};