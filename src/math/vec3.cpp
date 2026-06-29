#pragma once
#include <cmath>
#include <ostream>

namespace ext::math {

struct Vec3 {
    float x{}, y{}, z{};

    [[nodiscard]] Vec3 norm() const {
        float len = std::sqrt(x*x + y*y + z*z);
        if (len <= 0.00001f) return {0, 0, 0};
        return {x/len, y/len, z/len};
    }

    [[nodiscard]] float dot(const Vec3& b) const { return x*b.x + y*b.y + z*b.z; }

    [[nodiscard]] Vec3 cross(const Vec3& b) const {
        return {y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x};
    }
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
inline Vec3 operator*(const Vec3& a, float s) { return {a.x*s, a.y*s, a.z*s}; }
inline Vec3 operator*(float s, const Vec3& v) { return {s*v.x, s*v.y, s*v.z}; }
inline Vec3 operator/(const Vec3& a, float s) { return {a.x/s, a.y/s, a.z/s}; }
inline Vec3 operator-(const Vec3& a) { return {-a.x, -a.y, -a.z}; }

inline Vec3& operator+=(Vec3& a, const Vec3& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
inline Vec3& operator-=(Vec3& a, const Vec3& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
inline Vec3& operator*=(Vec3& a, float s) { a.x *= s; a.y *= s; a.z *= s; return a; }

inline bool operator==(const Vec3& a, const Vec3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
inline bool operator!=(const Vec3& a, const Vec3& b) { return !(a == b); }

inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << "Vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
}

} // namespace ext::math
