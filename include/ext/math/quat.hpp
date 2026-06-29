#pragma once
#include "vec3.hpp"
#include <cmath>
#include <ostream>

namespace ext::math {

struct Quat {
    float w{1}, x{0}, y{0}, z{0};

    [[nodiscard]] Quat operator*(const Quat& b) const {
        return {
            w*b.w - x*b.x - y*b.y - z*b.z,
            w*b.x + x*b.w + y*b.z - z*b.y,
            w*b.y - x*b.z + y*b.w + z*b.x,
            w*b.z + x*b.y - y*b.x + z*b.w
        };
    }

    [[nodiscard]] static Quat from_axis_angle(const Vec3& axis, float angle_rad) {
        Vec3 n = axis.norm();
        float half = 0.5f * angle_rad;
        float s = std::sin(half);
        float c = std::cos(half);
        return {c, n.x * s, n.y * s, n.z * s};
    }

    [[nodiscard]] Vec3 right() const {
        return {1 - 2*(y*y + z*z), 2*(x*y + w*z), 2*(x*z - w*y)};
    }

    [[nodiscard]] Vec3 forward() const {
        return {2.0f*(x*z + w*y), 2.0f*(y*z - w*x), -(1.0f - 2.0f*(x*x + y*y))};
    }

    [[nodiscard]] Vec3 up() const {
        return {2.0f*(x*y - w*z), 1.0f - 2.0f*(x*x + z*z), 2.0f*(y*z + w*x)};
    }

    [[nodiscard]] Quat norm() const {
        float len2 = x*x + y*y + z*z + w*w;
        float inv = 1.0f / std::sqrt(len2);
        return {w*inv, x*inv, y*inv, z*inv};
    }

    /// Extract quaternion from a pure rotation matrix (column-major, upper-left 3x3).
    [[nodiscard]] static Quat from_rotation_matrix(const float* m) {
        const float r00 = m[0], r01 = m[4], r02 = m[8];
        const float r10 = m[1], r11 = m[5], r12 = m[9];
        const float r20 = m[2], r21 = m[6], r22 = m[10];

        float qw, qx, qy, qz;
        const float trace = r00 + r11 + r22;

        if (trace > 0.0f) {
            float s = std::sqrt(trace + 1.0f) * 2.0f;
            qw = 0.25f * s;
            qx = (r21 - r12) / s;
            qy = (r02 - r20) / s;
            qz = (r10 - r01) / s;
        } else if (r00 > r11 && r00 > r22) {
            float s = std::sqrt(1.0f + r00 - r11 - r22) * 2.0f;
            qw = (r21 - r12) / s;
            qx = 0.25f * s;
            qy = (r01 + r10) / s;
            qz = (r02 + r20) / s;
        } else if (r11 > r22) {
            float s = std::sqrt(1.0f + r11 - r00 - r22) * 2.0f;
            qw = (r02 - r20) / s;
            qx = (r01 + r10) / s;
            qy = 0.25f * s;
            qz = (r12 + r21) / s;
        } else {
            float s = std::sqrt(1.0f + r22 - r00 - r11) * 2.0f;
            qw = (r10 - r01) / s;
            qx = (r02 + r20) / s;
            qy = (r12 + r21) / s;
            qz = 0.25f * s;
        }
        return {qw, qx, qy, qz};
    }
};

inline Vec3 operator*(const Quat& q, const Vec3& v) {
    Vec3 qv{q.x, q.y, q.z};
    Vec3 t = 2.0f * qv.cross(v);
    return {v.x + q.w * t.x + qv.cross(t).x,
            v.y + q.w * t.y + qv.cross(t).y,
            v.z + q.w * t.z + qv.cross(t).z};
}

inline std::ostream& operator<<(std::ostream& os, const Quat& q) {
    return os << "Quat(w=" << q.w << ", x=" << q.x << ", y=" << q.y << ", z=" << q.z << ")";
}

} // namespace ext::math
