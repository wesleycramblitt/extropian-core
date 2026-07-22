#include <doctest/doctest.h>
#include <exd/math/vec4.hpp>

using namespace exd::math;

TEST_CASE("Vec4 default construction") {
    Vec4f v;
    CHECK(v.x == 0.0f); CHECK(v.y == 0.0f);
    CHECK(v.z == 0.0f); CHECK(v.w == 0.0f);
}

TEST_CASE("Vec4 value construction") {
    Vec4f v{1, 2, 3, 4};
    CHECK(v.x == 1.0f); CHECK(v.y == 2.0f);
    CHECK(v.z == 3.0f); CHECK(v.w == 4.0f);
}

TEST_CASE("Vec4 addition") {
    auto r = Vec4f{1, 2, 3, 4} + Vec4f{5, 6, 7, 8};
    CHECK(r == Vec4f{6, 8, 10, 12});
}

TEST_CASE("Vec4 subtraction") {
    auto r = Vec4f{6, 8, 10, 12} - Vec4f{1, 2, 3, 4};
    CHECK(r == Vec4f{5, 6, 7, 8});
}

TEST_CASE("Vec4 scalar ops") {
    Vec4f v{1, 2, 3, 4};
    CHECK(v * 2.0f == Vec4f{2, 4, 6, 8});
    CHECK(v / 2.0f == Vec4f{0.5f, 1, 1.5f, 2});
    CHECK(-v == Vec4f{-1, -2, -3, -4});
}

TEST_CASE("Vec4 compound assignment") {
    Vec4f v{1, 2, 3, 4};
    v += Vec4f{1, 1, 1, 1}; CHECK(v == Vec4f{2, 3, 4, 5});
    v -= Vec4f{1, 1, 1, 1}; CHECK(v == Vec4f{1, 2, 3, 4});
    v *= 2.0f;              CHECK(v == Vec4f{2, 4, 6, 8});
    v /= 2.0f;              CHECK(v == Vec4f{1, 2, 3, 4});
}

TEST_CASE("Vec4 dot product") {
    CHECK(Vec4f{1, 2, 3, 4}.dot(Vec4f{5, 6, 7, 8}) == 70.0f);
    CHECK(Vec4f{1, 0, 0, 0}.dot(Vec4f{0, 1, 0, 0}) == 0.0f);
}

TEST_CASE("Vec4 length") {
    Vec4f v{2, 4, 4, 1};
    // 4 + 16 + 16 + 1 = 37, sqrt(37) ~ 6.0828
    CHECK(v.length_sq() == 37.0f);
    CHECK(v.length() == doctest::Approx(6.08276253f));
}

TEST_CASE("Vec4 normalized") {
    Vec4f v{2, 0, 0, 0};
    auto n = v.normalized();
    CHECK(n.length() == doctest::Approx(1.0f));
    CHECK(n.x == doctest::Approx(1.0f));
    CHECK(n.y == 0.0f); CHECK(n.z == 0.0f); CHECK(n.w == 0.0f);

    CHECK(Vec4f{}.normalized() == Vec4f{});
}

TEST_CASE("Vec4 distance") {
    CHECK(Vec4f{0, 0, 0, 0}.distance(Vec4f{3, 0, 4, 0}) == 5.0f);
}

TEST_CASE("Vec4 xyz access") {
    Vec4f v{1, 2, 3, 4};
    const float* xyz = v.xyz();
    CHECK(xyz[0] == 1.0f); CHECK(xyz[1] == 2.0f); CHECK(xyz[2] == 3.0f);
}

TEST_CASE("Vec4 subscript") {
    Vec4f v{10, 20, 30, 40};
    CHECK(v[0] == 10.0f); CHECK(v[1] == 20.0f);
    CHECK(v[2] == 30.0f); CHECK(v[3] == 40.0f);
    v[3] = 99.0f;
    CHECK(v.w == 99.0f);
}

TEST_CASE("Vec4 data pointer") {
    Vec4f v{1, 2, 3, 4};
    const float* d = v.data();
    CHECK(d[0] == 1.0f); CHECK(d[1] == 2.0f);
    CHECK(d[2] == 3.0f); CHECK(d[3] == 4.0f);
}

TEST_CASE("Vec4 static helpers") {
    CHECK(Vec4f::zero() == Vec4f{0, 0, 0, 0});
    CHECK(Vec4f::one()  == Vec4f{1, 1, 1, 1});
}

TEST_CASE("Vec4 scalar * Vec4") {
    CHECK(3.0f * Vec4f{1, 2, 3, 4} == Vec4f{3, 6, 9, 12});
}
