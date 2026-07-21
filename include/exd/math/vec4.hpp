#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>

namespace exd::math {

/// Four-component vector. Used for homogeneous coordinates and RGBA colors.
template <typename T>
struct Vec4 {
    T x{}, y{}, z{}, w{};

    // ── Arithmetic ──
    Vec4 operator+(const Vec4& o) const { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
    Vec4 operator-(const Vec4& o) const { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
    Vec4 operator*(T s) const { return {x * s, y * s, z * s, w * s}; }
    Vec4 operator/(T s) const { return {x / s, y / s, z / s, w / s}; }
    Vec4 operator-() const { return {-x, -y, -z, -w}; }

    Vec4& operator+=(const Vec4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
    Vec4& operator-=(const Vec4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
    Vec4& operator*=(T s) { x *= s; y *= s; z *= s; w *= s; return *this; }
    Vec4& operator/=(T s) { x /= s; y /= s; z /= s; w /= s; return *this; }

    // ── Comparison ──
    bool operator==(const Vec4& o) const = default;

    // ── Vector operations ──
    [[nodiscard]] T dot(const Vec4& o) const { return x * o.x + y * o.y + z * o.z + w * o.w; }
    [[nodiscard]] T length_sq() const { return dot(*this); }
    [[nodiscard]] T length() const { return std::sqrt(length_sq()); }
    [[nodiscard]] Vec4 normalized() const {
        T l = length();
        return l > T(0) ? *this / l : Vec4{};
    }
    [[nodiscard]] T distance(const Vec4& o) const { return (*this - o).length(); }

    /// Return the xyz portion as a 3-component view.
    /// Convenience for homogeneous coordinate usage.
    [[nodiscard]] const T* xyz() const { return &x; }

    // ── Subscript ──
    T& operator[](std::size_t i) { return (&x)[i]; }
    const T& operator[](std::size_t i) const { return (&x)[i]; }

    // ── Data access ──
    [[nodiscard]] const T* data() const { return &x; }
    [[nodiscard]] T* data() { return &x; }

    // ── Static helpers ──
    static Vec4 zero() { return {}; }
    static Vec4 one() { return {1, 1, 1, 1}; }
};

// Common aliases
using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;
using Vec4i = Vec4<int>;

// Scalar * Vec4
template <typename T>
Vec4<T> operator*(T s, const Vec4<T>& v) { return v * s; }

} // namespace exd::math
