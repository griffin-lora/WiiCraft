#pragma once
#include <cstdlib>
#include <gccore.h>
#include "glm/glm.hpp"

namespace math {
    using vector3s = glm::vec<3, s32, glm::defaultp>;
    using vector3u8 = glm::vec<3, u8, glm::defaultp>;
    using vector3s8 = glm::vec<3, s8, glm::defaultp>;
    using vector2u8 = glm::vec<2, u8, glm::defaultp>;
    using vector2s32 = glm::vec<2, s32, glm::defaultp>;
    using vector3s32 = glm::vec<3, s32, glm::defaultp>;
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

    template<typename T, glm::qualifier Q>
    inline T length_squared(const glm::vec<3, T, Q>& v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
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
    
    template<typename T>
    T mod(T a, T b) {
        T ret = a % b;
        return ret >= 0 ? ret : ret + b;
    }

    f32 get_noise_at(const glm::vec2& pos);
}