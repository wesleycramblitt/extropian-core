#pragma once
#include "quat.hpp"
#include "vec3.hpp"
#include <cmath>

namespace exd::math {

/// Dual quaternion for rigid-body transformations.
///
/// Represents a combined rotation and translation. Supports smooth
/// interpolation (scLERP) without the volume-collapse artifacts of
/// matrix or quaternion+vector linear blending — essential for
/// skeletal animation.
///
/// A dual quaternion is q = q_real + ε q_dual where ε² = 0.
///   q_real = rotation quaternion (unit)
///   q_dual = 0.5 * t * q_real  where t = (0, tx, ty, tz)
struct DualQuat {
    Quat real{1, 0, 0, 0};   ///< Rotation component
    Quat dual{0, 0, 0, 0};   ///< Translation component

    // ── Factories ──
    [[nodiscard]] static DualQuat identity() {
        return {{1, 0, 0, 0}, {0, 0, 0, 0}};
    }

    [[nodiscard]] static DualQuat from_rotation(const Quat& q) {
        Quat r = q.norm();
        return {r, {0, 0, 0, 0}};
    }

    [[nodiscard]] static DualQuat from_translation(const Vec3f& t) {
        return {
            {1, 0, 0, 0},
            {0, 0.5f * t.x, 0.5f * t.y, 0.5f * t.z}
        };
    }

    /// Compose a rigid transform: rotation then translation.
    [[nodiscard]] static DualQuat from_pose(const Quat& rot, const Vec3f& pos) {
        Quat r = rot.norm();
        // q_dual = 0.5 * (0, pos) * r
        Quat tq{0, pos.x, pos.y, pos.z};
        Quat d = tq * r;
        d.w *= 0.5f; d.x *= 0.5f; d.y *= 0.5f; d.z *= 0.5f;
        return {r, d};
    }

    /// Decompose into rotation and translation.
    void decompose(Quat& out_rot, Vec3f& out_trans) const {
        out_rot = real.norm();
        // translation = 2 * dual * conj(real)
        Quat cr{real.w, -real.x, -real.y, -real.z};
        Quat t = dual * cr;
        out_trans = {2.0f * t.x, 2.0f * t.y, 2.0f * t.z};
    }

    // ── Operations ──
    [[nodiscard]] DualQuat operator*(const DualQuat& b) const {
        // (a_r + ε a_d) * (b_r + ε b_d) = a_r*b_r + ε(a_r*b_d + a_d*b_r)
        return {
            real * b.real,
            real * b.dual + dual * b.real
        };
    }

    [[nodiscard]] DualQuat normalized() const {
        // Normalize by the real part's magnitude
        float len2 = real.w * real.w + real.x * real.x
                   + real.y * real.y + real.z * real.z;
        float inv = 1.0f / std::sqrt(len2);
        return {
            {real.w * inv, real.x * inv, real.y * inv, real.z * inv},
            {dual.w * inv, dual.x * inv, dual.y * inv, dual.z * inv}
        };
    }

    /// Transform a point by this dual quaternion.
    /// p' = q_real * p * q_real* + 2 * q_dual * q_real*
    [[nodiscard]] Vec3f transform(const Vec3f& p) const {
        // Rotate: r * p * r*  (using quat * vec3 helper)
        Vec3f rotated = real * p;

        // Translation: 2 * dual * conj(real) → extract xyz
        Quat cr{real.w, -real.x, -real.y, -real.z};
        Quat t = dual * cr;
        return {rotated.x + 2.0f * t.x,
                rotated.y + 2.0f * t.y,
                rotated.z + 2.0f * t.z};
    }

    // ── Interpolation ──
    /// Screw linear interpolation between two dual quaternions.
    /// @param t  Interpolation factor [0, 1].
    [[nodiscard]] static DualQuat sclerp(const DualQuat& a, const DualQuat& b, float t) {
        // Use shortest-path on the real part
        float dot = a.real.w * b.real.w + a.real.x * b.real.x
                  + a.real.y * b.real.y + a.real.z * b.real.z;
        float sign = dot < 0.0f ? -1.0f : 1.0f;

        DualQuat result;
        float blend = 1.0f - t;
        result.real.w = blend * a.real.w + t * sign * b.real.w;
        result.real.x = blend * a.real.x + t * sign * b.real.x;
        result.real.y = blend * a.real.y + t * sign * b.real.y;
        result.real.z = blend * a.real.z + t * sign * b.real.z;
        result.dual.w = blend * a.dual.w + t * sign * b.dual.w;
        result.dual.x = blend * a.dual.x + t * sign * b.dual.x;
        result.dual.y = blend * a.dual.y + t * sign * b.dual.y;
        result.dual.z = blend * a.dual.z + t * sign * b.dual.z;
        return result.normalized();
    }

    // ── Conversion ──
    /// Convert to a Mat4 (column-major, 16 floats). Writes into m[16].
    void to_mat4(float* m) const {
        Quat q = real.norm();
        const float x = q.x, y = q.y, z = q.z, w = q.w;
        const float xx = x*x, yy = y*y, zz = z*z;
        const float xy = x*y, xz = x*z, yz = y*z;
        const float wx = w*x, wy = w*y, wz = w*z;

        // Rotation 3x3
        m[0] = 1.0f - 2.0f*(yy + zz);
        m[1] = 2.0f*(xy + wz);
        m[2] = 2.0f*(xz - wy);
        m[3] = 0.0f;
        m[4] = 2.0f*(xy - wz);
        m[5] = 1.0f - 2.0f*(xx + zz);
        m[6] = 2.0f*(yz + wx);
        m[7] = 0.0f;
        m[8] = 2.0f*(xz + wy);
        m[9] = 2.0f*(yz - wx);
        m[10]= 1.0f - 2.0f*(xx + yy);
        m[11]= 0.0f;

        // Translation from dual part
        Quat cr{w, -x, -y, -z};
        Quat t = dual * cr;
        m[12] = 2.0f * t.x;
        m[13] = 2.0f * t.y;
        m[14] = 2.0f * t.z;
        m[15] = 1.0f;
    }

};

} // namespace exd::math
