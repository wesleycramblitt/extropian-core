#include <doctest/doctest.h>
#include <exd/math/dual_quat.hpp>
#include <cmath>

using namespace exd::math;

TEST_CASE("DualQuat identity") {
    auto dq = DualQuat::identity();
    CHECK(dq.real.w == 1.0f); CHECK(dq.real.x == 0.0f);
    CHECK(dq.real.y == 0.0f); CHECK(dq.real.z == 0.0f);
    CHECK(dq.dual.w == 0.0f); CHECK(dq.dual.x == 0.0f);
    CHECK(dq.dual.y == 0.0f); CHECK(dq.dual.z == 0.0f);
}

TEST_CASE("DualQuat from_rotation") {
    auto q = Quat::from_axis_angle(Vec3f{0, 0, 1}, float(M_PI / 2.0));
    auto dq = DualQuat::from_rotation(q);
    // Should have the rotation quat (normalized) and dual zeroed
    CHECK(dq.dual.w == 0.0f); CHECK(dq.dual.x == 0.0f);
    CHECK(dq.dual.y == 0.0f); CHECK(dq.dual.z == 0.0f);
    // real part should be normalized
    float len = std::sqrt(dq.real.w*dq.real.w + dq.real.x*dq.real.x +
                          dq.real.y*dq.real.y + dq.real.z*dq.real.z);
    CHECK(len == doctest::Approx(1.0f));
}

TEST_CASE("DualQuat from_translation") {
    auto dq = DualQuat::from_translation(Vec3f{2, 4, 6});
    CHECK(dq.real.w == 1.0f); CHECK(dq.real.x == 0.0f);
    CHECK(dq.real.y == 0.0f); CHECK(dq.real.z == 0.0f);
    CHECK(dq.dual.w == 0.0f);
    CHECK(dq.dual.x == doctest::Approx(1.0f)); // 0.5 * 2
    CHECK(dq.dual.y == doctest::Approx(2.0f)); // 0.5 * 4
    CHECK(dq.dual.z == doctest::Approx(3.0f)); // 0.5 * 6
}

TEST_CASE("DualQuat from_pose decompose round-trip") {
    Quat rot_in = Quat::from_axis_angle(Vec3f{1, 0, 0}, 0.5f);
    Vec3f pos_in{10, 20, 30};

    auto dq = DualQuat::from_pose(rot_in, pos_in);

    Quat rot_out;
    Vec3f pos_out;
    dq.decompose(rot_out, pos_out);

    CHECK(pos_out.x == doctest::Approx(pos_in.x));
    CHECK(pos_out.y == doctest::Approx(pos_in.y));
    CHECK(pos_out.z == doctest::Approx(pos_in.z));

    // Rotation should match (may differ by sign)
    float dot = rot_in.w * rot_out.w + rot_in.x * rot_out.x +
                rot_in.y * rot_out.y + rot_in.z * rot_out.z;
    CHECK(std::abs(dot) == doctest::Approx(1.0f));
}

TEST_CASE("DualQuat multiply composition") {
    // Compose: translate then rotate
    auto trans = DualQuat::from_translation(Vec3f{5, 0, 0});
    auto rot = DualQuat::from_rotation(
        Quat::from_axis_angle(Vec3f{0, 0, 1}, float(M_PI / 2.0))
    );

    // Apply rotation then translation
    auto composed = trans * rot;

    // Transform a point at origin
    auto result = composed.transform(Vec3f{1, 0, 0});
    // (1,0,0) rotated 90° around Z → (0,1,0), then translated (5,0,0) → (5,1,0)
    CHECK(result.x == doctest::Approx(5.0f).epsilon(1e-4f));
    CHECK(result.y == doctest::Approx(1.0f).epsilon(1e-4f));
    CHECK(result.z == doctest::Approx(0.0f).epsilon(1e-4f));
}

TEST_CASE("DualQuat transform identity") {
    auto dq = DualQuat::identity();
    Vec3f p{1, 2, 3};
    auto r = dq.transform(p);
    CHECK(r.x == doctest::Approx(p.x));
    CHECK(r.y == doctest::Approx(p.y));
    CHECK(r.z == doctest::Approx(p.z));
}

TEST_CASE("DualQuat transform translation") {
    auto dq = DualQuat::from_translation(Vec3f{10, 20, 30});
    Vec3f p{1, 2, 3};
    auto r = dq.transform(p);
    CHECK(r.x == doctest::Approx(11.0f));
    CHECK(r.y == doctest::Approx(22.0f));
    CHECK(r.z == doctest::Approx(33.0f));
}

TEST_CASE("DualQuat normalized") {
    auto dq = DualQuat::from_translation(Vec3f{1, 2, 3});
    // from_translation produces a unit real part already
    auto n = dq.normalized();
    float len = std::sqrt(n.real.w*n.real.w + n.real.x*n.real.x +
                          n.real.y*n.real.y + n.real.z*n.real.z);
    CHECK(len == doctest::Approx(1.0f));
}

TEST_CASE("DualQuat sclerp") {
    auto a = DualQuat::from_translation(Vec3f{0, 0, 0});
    auto b = DualQuat::from_translation(Vec3f{10, 0, 0});

    // At t=0, should be at a
    auto s0 = DualQuat::sclerp(a, b, 0.0f);
    auto p0 = s0.transform(Vec3f{0, 0, 0});
    CHECK(p0.x == doctest::Approx(0.0f));

    // At t=1, should be at b
    auto s1 = DualQuat::sclerp(a, b, 1.0f);
    auto p1 = s1.transform(Vec3f{0, 0, 0});
    CHECK(p1.x == doctest::Approx(10.0f));

    // At t=0.5, should be halfway
    auto s05 = DualQuat::sclerp(a, b, 0.5f);
    auto p05 = s05.transform(Vec3f{0, 0, 0});
    CHECK(p05.x == doctest::Approx(5.0f));
}

TEST_CASE("DualQuat to_mat4") {
    auto dq = DualQuat::from_translation(Vec3f{5, 10, 15});
    float m[16] = {};
    dq.to_mat4(m);

    // Column-major: translation in elements 12,13,14
    CHECK(m[12] == doctest::Approx(5.0f));
    CHECK(m[13] == doctest::Approx(10.0f));
    CHECK(m[14] == doctest::Approx(15.0f));
    CHECK(m[15] == doctest::Approx(1.0f));

    // Upper-left 3x3 should be identity for pure translation
    CHECK(m[0]  == doctest::Approx(1.0f));
    CHECK(m[5]  == doctest::Approx(1.0f));
    CHECK(m[10] == doctest::Approx(1.0f));
    CHECK(m[1]  == doctest::Approx(0.0f));
    CHECK(m[2]  == doctest::Approx(0.0f));
    CHECK(m[4]  == doctest::Approx(0.0f));
}
