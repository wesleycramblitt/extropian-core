#pragma once
#include "vec3.hpp"
#include <cmath>
#include <iostream>

namespace exd::math {

/// Column-major 3x3 matrix (9 floats). Represents linear transforms
/// (rotation, scale, shear) without translation.
struct Mat3 {
    float m[9]{};

    [[nodiscard]] static Mat3 identity() {
        Mat3 r{};
        r.m[0] = r.m[4] = r.m[8] = 1.0f;
        return r;
    }

    [[nodiscard]] static Mat3 zero() {
        return Mat3{};
    }

    // ── Matrix multiplication ──
    [[nodiscard]] static Mat3 mul(const Mat3& a, const Mat3& b) {
        Mat3 r{};
        for (int c = 0; c < 3; ++c)
            for (int row = 0; row < 3; ++row)
                r.m[c * 3 + row] =
                    a.m[0 * 3 + row] * b.m[c * 3 + 0] +
                    a.m[1 * 3 + row] * b.m[c * 3 + 1] +
                    a.m[2 * 3 + row] * b.m[c * 3 + 2];
        return r;
    }

    // ── Transforms ──
    [[nodiscard]] static Mat3 transpose(const Mat3& a) {
        Mat3 r{};
        r.m[0] = a.m[0]; r.m[1] = a.m[3]; r.m[2] = a.m[6];
        r.m[3] = a.m[1]; r.m[4] = a.m[4]; r.m[5] = a.m[7];
        r.m[6] = a.m[2]; r.m[7] = a.m[5]; r.m[8] = a.m[8];
        return r;
    }

    // ── Determinant ──
    [[nodiscard]] static float det(const Mat3& a) {
        return a.m[0] * (a.m[4] * a.m[8] - a.m[5] * a.m[7])
             - a.m[1] * (a.m[3] * a.m[8] - a.m[5] * a.m[6])
             + a.m[2] * (a.m[3] * a.m[7] - a.m[4] * a.m[6]);
    }

    // ── Inverse (general) ──
    [[nodiscard]] static Mat3 inverse(const Mat3& a) {
        float inv_det = 1.0f / det(a);
        Mat3 r{};
        r.m[0] =  (a.m[4] * a.m[8] - a.m[5] * a.m[7]) * inv_det;
        r.m[1] = -(a.m[1] * a.m[8] - a.m[2] * a.m[7]) * inv_det;
        r.m[2] =  (a.m[1] * a.m[5] - a.m[2] * a.m[4]) * inv_det;
        r.m[3] = -(a.m[3] * a.m[8] - a.m[5] * a.m[6]) * inv_det;
        r.m[4] =  (a.m[0] * a.m[8] - a.m[2] * a.m[6]) * inv_det;
        r.m[5] = -(a.m[0] * a.m[5] - a.m[2] * a.m[3]) * inv_det;
        r.m[6] =  (a.m[3] * a.m[7] - a.m[4] * a.m[6]) * inv_det;
        r.m[7] = -(a.m[0] * a.m[7] - a.m[1] * a.m[6]) * inv_det;
        r.m[8] =  (a.m[0] * a.m[4] - a.m[1] * a.m[3]) * inv_det;
        return r;
    }

    // ── Builders ──
    /// Build a 3x3 rotation matrix from axis + angle.
    [[nodiscard]] static Mat3 from_axis_angle(const Vec3f& axis, float angle_rad) {
        Vec3f n = axis.normalized();
        float c = std::cos(angle_rad);
        float s = std::sin(angle_rad);
        float t = 1.0f - c;
        Mat3 r{};
        r.m[0] = t * n.x * n.x + c;
        r.m[1] = t * n.x * n.y + s * n.z;
        r.m[2] = t * n.x * n.z - s * n.y;
        r.m[3] = t * n.x * n.y - s * n.z;
        r.m[4] = t * n.y * n.y + c;
        r.m[5] = t * n.y * n.z + s * n.x;
        r.m[6] = t * n.x * n.z + s * n.y;
        r.m[7] = t * n.y * n.z - s * n.x;
        r.m[8] = t * n.z * n.z + c;
        return r;
    }

    /// Build a 3x3 scale matrix.
    [[nodiscard]] static Mat3 from_scale(const Vec3f& scale) {
        Mat3 r{};
        r.m[0] = scale.x;
        r.m[4] = scale.y;
        r.m[8] = scale.z;
        return r;
    }

    /// Extract the upper-left 3x3 from a column-major 4x4 matrix (elements m[16]).
    [[nodiscard]] static Mat3 from_mat4(const float* mat4) {
        Mat3 r{};
        r.m[0] = mat4[0];  r.m[3] = mat4[4];  r.m[6] = mat4[8];
        r.m[1] = mat4[1];  r.m[4] = mat4[5];  r.m[7] = mat4[9];
        r.m[2] = mat4[2];  r.m[5] = mat4[6];  r.m[8] = mat4[10];
        return r;
    }

    // ── Vector transform ──
    /// Multiply matrix by vector:  M * v
    [[nodiscard]] static Vec3f mul_vec(const Mat3& a, const Vec3f& v) {
        return {
            a.m[0] * v.x + a.m[3] * v.y + a.m[6] * v.z,
            a.m[1] * v.x + a.m[4] * v.y + a.m[7] * v.z,
            a.m[2] * v.x + a.m[5] * v.y + a.m[8] * v.z
        };
    }

    void print() const {
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col)
                std::cout << m[col * 3 + row] << " ";
            std::cout << "\n";
        }
    }
};

} // namespace exd::math
