#pragma once
#include "vector.hpp"

namespace math {
    template<typename T>
    struct matrix3 {
        vector3<T> vec1;
        vector3<T> vec2;
        vector3<T> vec3;

        inline auto& look() {
            return vec1;
        }

        inline auto& up() {
            return vec2;
        }

        inline auto& right() {
            return vec3;
        }

        inline const auto& look() const {
            return vec1;
        }

        inline const auto& up() const {
            return vec2;
        }

        inline const auto& right() const {
            return vec3;
        }

        matrix3 operator*(const matrix3& other) const {
            return {
                other.vec1 * vec1.x + other.vec2 * vec1.y + other.vec3 * vec1.z,
                other.vec1 * vec2.x + other.vec2 * vec2.y + other.vec3 * vec2.z,
                other.vec1 * vec3.x + other.vec2 * vec3.y + other.vec3 * vec3.z
            };
        }

        vector3<T> operator*(const vector3<T>& other) const {
            return vec1 * other.x + vec2 * other.y + vec3 * other.z;
        }

        void normalize() {
            vec1.normalize();
            vec2.normalize();
            vec3.normalize();
        }
    };

    template<typename T>
    struct matrix4 {
        vector4<T> vec1;
        vector4<T> vec2;
        vector4<T> vec3;
        vector4<T> vec4;

        matrix4 operator*(const matrix4& other) const {
            return {
                other.vec1 * vec1.x + other.vec2 * vec1.y + other.vec3 * vec1.z + other.vec4 * vec1.w,
                other.vec1 * vec2.x + other.vec2 * vec2.y + other.vec3 * vec2.z + other.vec4 * vec2.w,
                other.vec1 * vec3.x + other.vec2 * vec3.y + other.vec3 * vec3.z + other.vec4 * vec3.w,
                other.vec1 * vec4.x + other.vec2 * vec4.y + other.vec3 * vec4.z + other.vec4 * vec4.w
            };
        }

        vector4<T> operator*(const vector4<T>& other) const {
            return vec1 * other.x + vec2 * other.y + vec3 * other.z + vec4 * other.w;
        }

        void normalize() {
            vec1.normalize();
            vec2.normalize();
            vec3.normalize();
            vec4.normalize();
        }
    };
}