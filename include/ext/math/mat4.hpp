#pragma once
#include "vec3.hpp"
#include "quat.hpp"
#include <cmath>
#include <iostream>

namespace ext::math {

/// Column-major 4x4 matrix (16 floats).
struct Mat4 {
    float m[16]{};

    [[nodiscard]] static Mat4 identity() {
        Mat4 r{};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    void print() const {
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col)
                std::cout << m[col * 4 + row] << " ";
            std::cout << "\n";
        }
    }

    [[nodiscard]] static Mat4 mul(const Mat4& a, const Mat4& b) {
        Mat4 r{};
        for (int c = 0; c < 4; c++)
            for (int r0 = 0; r0 < 4; r0++)
                r.m[c*4 + r0] =
                    a.m[0*4 + r0]*b.m[c*4 + 0] +
                    a.m[1*4 + r0]*b.m[c*4 + 1] +
                    a.m[2*4 + r0]*b.m[c*4 + 2] +
                    a.m[3*4 + r0]*b.m[c*4 + 3];
        return r;
    }

    [[nodiscard]] static Mat4 perspective(float fovy_rad, float aspect, float znear, float zfar) {
        float f = 1.0f / std::tan(fovy_rad * 0.5f);
        Mat4 r{};
        r.m[0]  = f / aspect;
        r.m[5]  = f;
        r.m[10] = (zfar + znear) / (znear - zfar);
        r.m[11] = -1.0f;
        r.m[14] = (2.0f * zfar * znear) / (znear - zfar);
        return r;
    }

    [[nodiscard]] static Mat4 look_at(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).norm();
        Vec3 s = f.cross(up).norm();
        Vec3 u = s.cross(f).norm();

        Mat4 r = identity();
        r.m[0]  = s.x;  r.m[4]  = s.y;  r.m[8]  = s.z;
        r.m[1]  = u.x;  r.m[5]  = u.y;  r.m[9]  = u.z;
        r.m[2]  = -f.x; r.m[6]  = -f.y; r.m[10] = -f.z;
        r.m[12] = -s.dot(eye);
        r.m[13] = -u.dot(eye);
        r.m[14] =  f.dot(eye);
        return r;
    }

    /// Build model matrix: Translation * Rotation * Scale
    [[nodiscard]] static Mat4 trs(const Vec3& pos, const Quat& rot, const Vec3& scale) {
        Quat q = rot.norm();
        const float x = q.x, y = q.y, z = q.z, w = q.w;
        const float xx = x*x, yy = y*y, zz = z*z;
        const float xy = x*y, xz = x*z, yz = y*z;
        const float wx = w*x, wy = w*y, wz = w*z;

        Mat4 M{};
        M.m[0]  = (1.0f - 2.0f*(yy + zz)) * scale.x;
        M.m[1]  = (2.0f*(xy + wz))        * scale.x;
        M.m[2]  = (2.0f*(xz - wy))        * scale.x;
        M.m[3]  = 0.0f;
        M.m[4]  = (2.0f*(xy - wz))        * scale.y;
        M.m[5]  = (1.0f - 2.0f*(xx + zz))* scale.y;
        M.m[6]  = (2.0f*(yz + wx))        * scale.y;
        M.m[7]  = 0.0f;
        M.m[8]  = (2.0f*(xz + wy))        * scale.z;
        M.m[9]  = (2.0f*(yz - wx))        * scale.z;
        M.m[10] = (1.0f - 2.0f*(xx + yy))* scale.z;
        M.m[11] = 0.0f;
        M.m[12] = pos.x; M.m[13] = pos.y; M.m[14] = pos.z; M.m[15] = 1.0f;
        return M;
    }

    /// Build model matrix with skew: T * R * K * S
    [[nodiscard]] static Mat4 trs(const Vec3& pos, const Quat& rot,
                                   const Vec3& scale, const Vec3& skew) {
        Quat q = rot.norm();
        const float x = q.x, y = q.y, z = q.z, w = q.w;
        const float xx = x*x, yy = y*y, zz = z*z;
        const float xy = x*y, xz = x*z, yz = y*z;
        const float wx = w*x, wy = w*y, wz = w*z;

        const float r00 = 1.0f - 2.0f*(yy + zz);
        const float r01 = 2.0f*(xy - wz);
        const float r02 = 2.0f*(xz + wy);
        const float r10 = 2.0f*(xy + wz);
        const float r11 = 1.0f - 2.0f*(xx + zz);
        const float r12 = 2.0f*(yz - wx);
        const float r20 = 2.0f*(xz - wy);
        const float r21 = 2.0f*(yz + wx);
        const float r22 = 1.0f - 2.0f*(xx + yy);

        const float kxy = skew.x, kxz = skew.y, kyz = skew.z;

        Mat4 M{};
        M.m[0] = r00 * scale.x;
        M.m[1] = r10 * scale.x;
        M.m[2] = r20 * scale.x;
        M.m[3] = 0.0f;
        M.m[4] = (r00 * kxy + r01) * scale.y;
        M.m[5] = (r10 * kxy + r11) * scale.y;
        M.m[6] = (r20 * kxy + r21) * scale.y;
        M.m[7] = 0.0f;
        M.m[8]  = (r00 * kxz + r01 * kyz + r02) * scale.z;
        M.m[9]  = (r10 * kxz + r11 * kyz + r12) * scale.z;
        M.m[10] = (r20 * kxz + r21 * kyz + r22) * scale.z;
        M.m[11] = 0.0f;
        M.m[12] = pos.x; M.m[13] = pos.y; M.m[14] = pos.z; M.m[15] = 1.0f;
        return M;
    }
};

} // namespace ext::math
