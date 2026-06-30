#pragma once

#include <cmath>
#include <cstddef>
#include <array>
#include <type_traits>

namespace exd::math {

/// Three-component vector. The foundational type for all geometry.
template <typename T>
struct Vec3 {
    T x{}, y{}, z{};

    // ── Arithmetic ──
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(T s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(T s) const { return {x / s, y / s, z / s}; }
    Vec3 operator-() const { return {-x, -y, -z}; }

    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(T s) { x *= s; y *= s; z *= s; return *this; }
    Vec3& operator/=(T s) { x /= s; y /= s; z /= s; return *this; }

    // ── Comparison ──
    bool operator==(const Vec3& o) const = default;

    // ── Vector operations ──
    [[nodiscard]] T dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    [[nodiscard]] Vec3 cross(const Vec3& o) const {
        return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
    }
    [[nodiscard]] T length_sq() const { return dot(*this); }
    [[nodiscard]] T length() const { return std::sqrt(length_sq()); }
    [[nodiscard]] Vec3 normalized() const {
        T l = length();
        return l > T(0) ? *this / l : Vec3{};
    }
    [[nodiscard]] T distance(const Vec3& o) const { return (*this - o).length(); }

    // ── Subscript ──
    T& operator[](std::size_t i) { return (&x)[i]; }
    const T& operator[](std::size_t i) const { return (&x)[i]; }

    // ── Data access ──
    [[nodiscard]] const T* data() const { return &x; }
    [[nodiscard]] T* data() { return &x; }

    // ── Static helpers ──
    static Vec3 zero() { return {}; }
    static Vec3 one() { return {1, 1, 1}; }
    static Vec3 up() { return {0, 1, 0}; }
    static Vec3 forward() { return {0, 0, -1}; }
    static Vec3 right() { return {1, 0, 0}; }
};

// Common aliases (Vec3 is a template; use Vec3f for float, Vec3d for double, Vec3i for int)
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<int>;

// Scalar * Vec3
template <typename T>
Vec3<T> operator*(T s, const Vec3<T>& v) { return v * s; }

} // namespace exd::math
