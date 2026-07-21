#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>

namespace exd::math {

/// Two-component vector. Foundation for 2D geometry, UVs, screen coordinates.
template <typename T>
struct Vec2 {
    T x{}, y{};

    // ── Arithmetic ──
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(T s) const { return {x * s, y * s}; }
    Vec2 operator/(T s) const { return {x / s, y / s}; }
    Vec2 operator-() const { return {-x, -y}; }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(T s) { x *= s; y *= s; return *this; }
    Vec2& operator/=(T s) { x /= s; y /= s; return *this; }

    // ── Comparison ──
    bool operator==(const Vec2& o) const = default;

    // ── Vector operations ──
    [[nodiscard]] T dot(const Vec2& o) const { return x * o.x + y * o.y; }
    [[nodiscard]] T cross(const Vec2& o) const { return x * o.y - y * o.x; }
    [[nodiscard]] T length_sq() const { return dot(*this); }
    [[nodiscard]] T length() const { return std::sqrt(length_sq()); }
    [[nodiscard]] Vec2 normalized() const {
        T l = length();
        return l > T(0) ? *this / l : Vec2{};
    }
    [[nodiscard]] T distance(const Vec2& o) const { return (*this - o).length(); }

    // ── Subscript ──
    T& operator[](std::size_t i) { return (&x)[i]; }
    const T& operator[](std::size_t i) const { return (&x)[i]; }

    // ── Data access ──
    [[nodiscard]] const T* data() const { return &x; }
    [[nodiscard]] T* data() { return &x; }

    // ── Static helpers ──
    static Vec2 zero() { return {}; }
    static Vec2 one() { return {1, 1}; }
    static Vec2 up() { return {0, 1}; }
    static Vec2 right() { return {1, 0}; }
};

// Common aliases
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int>;

// Scalar * Vec2
template <typename T>
Vec2<T> operator*(T s, const Vec2<T>& v) { return v * s; }

} // namespace exd::math
