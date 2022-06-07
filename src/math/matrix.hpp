#pragma once
#include "vector.hpp"

namespace math {
    template<typename T>
    struct matrix3 {
        vector3<T> vec1;
        vector3<T> vec2;
        vector3<T> vec3;

        inline matrix3 operator*(const matrix3& other) const {
            return {
                other.vec1 * vec1.x + other.vec2 * vec1.y + other.vec3 * vec1.z,
                other.vec1 * vec2.x + other.vec2 * vec2.y + other.vec3 * vec2.z,
                other.vec1 * vec3.x + other.vec2 * vec3.y + other.vec3 * vec3.z
            };
        }

        inline vector3<T> operator*(const vector3<T>& other) const {
            return vec1 * other.x + vec2 * other.y + vec3 * other.z;
        }

        inline matrix3 operator*(T scalar) const {
            return { vec1 * scalar, vec2 * scalar, vec3 * scalar };
        }

        inline matrix3& operator*=(const matrix3& other) {
            vec1 = other.vec1 * vec1.x + other.vec2 * vec1.y + other.vec3 * vec1.z;
            vec2 = other.vec1 * vec2.x + other.vec2 * vec2.y + other.vec3 * vec2.z;
            vec3 = other.vec1 * vec3.x + other.vec2 * vec3.y + other.vec3 * vec3.z;
            return *this;
        }

        inline matrix3& operator*=(T scalar) {
            vec1 *= scalar;
            vec2 *= scalar;
            vec3 *= scalar;
            return *this;
        }

        inline void normalize() {
            vec1.normalize();
            vec2.normalize();
            vec3.normalize();
        }
    };
}