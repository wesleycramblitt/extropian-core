#include <doctest/doctest.h>
#include <exd/math/quat.hpp>
#include <cmath>
#include <sstream>

using namespace exd::math;

TEST_CASE("Quat default is identity") {
    Quat q;
    CHECK(q.w == 1.0f); CHECK(q.x == 0.0f);
    CHECK(q.y == 0.0f); CHECK(q.z == 0.0f);
}

TEST_CASE("Quat multiply by identity") {
    Quat id{1, 0, 0, 0};
    Quat q{0.5f, 0.5f, 0.5f, 0.5f};
    auto r = q * id;
    CHECK(r.w == q.w); CHECK(r.x == q.x);
    CHECK(r.y == q.y); CHECK(r.z == q.z);
}

TEST_CASE("Quat addition/subtraction") {
    Quat a{1, 2, 3, 4}, b{5, 6, 7, 8};
    auto sum = a + b;
    CHECK(sum.w == 6); CHECK(sum.x == 8); CHECK(sum.y == 10); CHECK(sum.z == 12);

    auto diff = a - b;
    CHECK(diff.w == -4); CHECK(diff.x == -4); CHECK(diff.y == -4); CHECK(diff.z == -4);
}

TEST_CASE("Quat norm") {
    Quat q{1, 2, 3, 4};
    auto n = q.norm();
    float len = std::sqrt(n.w*n.w + n.x*n.x + n.y*n.y + n.z*n.z);
    CHECK(len == doctest::Approx(1.0f));

    // Already unit
    Quat unit{1, 0, 0, 0};
    auto nu = unit.norm();
    CHECK(nu.w == doctest::Approx(1.0f));
    CHECK(nu.x == doctest::Approx(0.0f));
    CHECK(nu.y == doctest::Approx(0.0f));
    CHECK(nu.z == doctest::Approx(0.0f));
}

TEST_CASE("Quat from_axis_angle") {
    // 90 degrees around Z
    auto q = Quat::from_axis_angle(Vec3f{0, 0, 1}, float(M_PI / 2.0));
    // cos(45°), 0, 0, sin(45°)
    float expected = std::sqrt(2.0f) / 2.0f;
    CHECK(q.w == doctest::Approx(expected));
    CHECK(q.x == doctest::Approx(0.0f));
    CHECK(q.y == doctest::Approx(0.0f));
    CHECK(q.z == doctest::Approx(expected));
}

TEST_CASE("Quat direction vectors") {
    // Identity quaternion
    Quat id{1, 0, 0, 0};
    CHECK(id.right()   == Vec3f{1, 0, 0});
    CHECK(id.up()      == Vec3f{0, 1, 0});
    CHECK(id.forward().x == doctest::Approx(0.0f));
    CHECK(id.forward().y == doctest::Approx(0.0f));
    CHECK(id.forward().z == doctest::Approx(-1.0f));
}

TEST_CASE("Quat rotate vector") {
    // 90 degrees around Z axis
    auto q = Quat::from_axis_angle(Vec3f{0, 0, 1}, float(M_PI / 2.0));
    Vec3f v{1, 0, 0};
    auto r = q * v;
    CHECK(r.x == doctest::Approx(0.0f).epsilon(1e-5f));
    CHECK(r.y == doctest::Approx(1.0f).epsilon(1e-5f));
    CHECK(r.z == doctest::Approx(0.0f).epsilon(1e-5f));
}

TEST_CASE("Quat from_rotation_matrix round-trip") {
    // Create a rotation around X by 90 degrees
    auto q_in = Quat::from_axis_angle(Vec3f{1, 0, 0}, float(M_PI / 2.0));

    // Build a rotation matrix from the quat
    float m[16] = {};
    // Use the same logic as to_mat4 from DualQuat
    Quat q = q_in.norm();
    float x = q.x, y = q.y, z = q.z, w = q.w;
    float xx = x*x, yy = y*y, zz = z*z;
    float xy = x*y, xz = x*z, yz = y*z;
    float wx = w*x, wy = w*y, wz = w*z;
    m[0] = 1 - 2*(yy+zz); m[4] = 2*(xy - wz);   m[8]  = 2*(xz + wy);
    m[1] = 2*(xy + wz);    m[5] = 1 - 2*(xx+zz); m[9]  = 2*(yz - wx);
    m[2] = 2*(xz - wy);    m[6] = 2*(yz + wx);   m[10] = 1 - 2*(xx+yy);
    m[15] = 1;

    auto q_out = Quat::from_rotation_matrix(m);
    // May differ by sign — check that the absolute values match
    // Use dot product to check they represent the same rotation
    float dot = q_in.w * q_out.w + q_in.x * q_out.x + q_in.y * q_out.y + q_in.z * q_out.z;
    CHECK(std::abs(dot) == doctest::Approx(1.0f));
}

TEST_CASE("Quat ostream") {
    Quat q{1, 2, 3, 4};
    std::ostringstream ss;
    ss << q;
    CHECK(!ss.str().empty());
}
