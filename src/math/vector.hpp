#pragma once
#include <cstdint>
#include <cmath>
#include <array>
#include <vector>
#include <ogc/gu.h>

namespace math {
    template<typename T>
    struct vector2 {
        T x;
        T y;

        inline vector2 operator+(const vector2& other) const {
            return {x + other.x, y + other.y};
        }
        inline vector2 operator-(const vector2& other) const {
            return {x - other.x, y - other.y};
        }
        inline vector2 operator*(const vector2& other) const {
            return {x * other.x, y * other.y};
        }
        inline vector2 operator/(const vector2& other) const {
            return {x / other.x, y / other.y};
        }
        inline vector2 operator*(T scalar) const {
            return {x * scalar, y * scalar};
        }
        inline vector2 operator/(T scalar) const {
            return {x / scalar, y / scalar};
        }

        inline vector2& operator+=(const vector2& other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        inline vector2& operator-=(const vector2& other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        inline vector2& operator*=(const vector2& other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        inline vector2& operator/=(const vector2& other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        inline vector2& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        inline vector2& operator/=(T scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        inline bool operator==(const vector2& other) const {
            return x == other.x && y == other.y;
        }

        inline bool operator!=(const vector2& other) const {
            return x != other.x || y != other.y;
        }

        inline bool is_non_zero() const {
            return x != 0 || y != 0;
        }

        inline f32 magnitude() const {
            return std::sqrt(x * x + y * y);
        }

        inline void normalize() {
            auto mag = magnitude();
            x /= mag;
            y /= mag;
        }
    };

    template<typename T>
    struct vector3 {
        T x;
        T y;
        T z;

        inline operator guVector() const {
            return {x, y, z};
        }

        inline vector3 operator+(const vector3& other) const {
            return {x + other.x, y + other.y, z + other.z};
        }
        inline vector3 operator-(const vector3& other) const {
            return {x - other.x, y - other.y, z - other.z};
        }
        inline vector3 operator*(const vector3& other) const {
            return {x * other.x, y * other.y, z * other.z};
        }
        inline vector3 operator/(const vector3& other) const {
            return {x / other.x, y / other.y, z / other.z};
        }
        inline vector3 operator*(T scalar) const {
            return {x * scalar, y * scalar, z * scalar};
        }
        inline vector3 operator/(T scalar) const {
            return {x / scalar, y / scalar, z / scalar};
        }

        inline vector3& operator+=(const vector3& other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        inline vector3& operator-=(const vector3& other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        inline vector3& operator*=(const vector3& other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        inline vector3& operator/=(const vector3& other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        inline vector3& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        inline vector3& operator/=(T scalar) {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        inline bool operator==(const vector3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        inline bool operator!=(const vector3& other) const {
            return x != other.x || y != other.y || z != other.z;
        }

        inline bool is_non_zero() const {
            return x != 0 || y != 0 || z != 0;
        }

        inline f32 magnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        inline void normalize() {
            auto mag = magnitude();
            x /= mag;
            y /= mag;
            z /= mag;
        }
    };
}