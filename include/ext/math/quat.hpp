#pragma once

#include <ext/math/vec3.hpp>
#include <cmath>

namespace ext::math {

/// Quaternion for 3D rotations.
template <typename T>
struct Quat {
    T w{1}, x{0}, y{0}, z{0};

    static Quat identity() { return {1, 0, 0, 0}; }
    static Quat from_axis_angle(const Vec3<T>& axis, T angle);
    static Quat from_euler(T pitch, T yaw, T roll);

    [[nodiscard]] Quat normalized() const;
    [[nodiscard]] Vec3<T> rotate(const Vec3<T>& v) const;
    [[nodiscard]] Quat operator*(const Quat& o) const;
};

using Quatf = Quat<float>;
using Quatd = Quat<double>;

} // namespace ext::math
