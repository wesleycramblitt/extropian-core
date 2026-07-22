#include <doctest/doctest.h>
#include <exd/math/vec3.hpp>

using namespace exd::math;

TEST_CASE("Vec3 default construction") {
    Vec3f v;
    CHECK(v.x == 0.0f);
    CHECK(v.y == 0.0f);
    CHECK(v.z == 0.0f);
}

TEST_CASE("Vec3 value construction") {
    Vec3f v{1, 2, 3};
    CHECK(v.x == 1.0f); CHECK(v.y == 2.0f); CHECK(v.z == 3.0f);
}

TEST_CASE("Vec3 addition") {
    Vec3f a{1, 2, 3}, b{4, 5, 6};
    auto c = a + b;
    CHECK(c == Vec3f{5, 7, 9});
}

TEST_CASE("Vec3 subtraction") {
    auto c = Vec3f{5, 7, 9} - Vec3f{1, 2, 3};
    CHECK(c == Vec3f{4, 5, 6});
}

TEST_CASE("Vec3 scalar multiply/divide") {
    Vec3f v{2, 4, 6};
    CHECK(v * 2.0f == Vec3f{4, 8, 12});
    CHECK(v / 2.0f == Vec3f{1, 2, 3});
}

TEST_CASE("Vec3 unary negation") {
    CHECK(-Vec3f{1, -2, 3} == Vec3f{-1, 2, -3});
}

TEST_CASE("Vec3 compound assignment") {
    Vec3f v{1, 2, 3};
    v += Vec3f{1, 1, 1}; CHECK(v == Vec3f{2, 3, 4});
    v -= Vec3f{1, 1, 1}; CHECK(v == Vec3f{1, 2, 3});
    v *= 2.0f;           CHECK(v == Vec3f{2, 4, 6});
    v /= 2.0f;           CHECK(v == Vec3f{1, 2, 3});
}

TEST_CASE("Vec3 dot product") {
    CHECK(Vec3f{1, 0, 0}.dot(Vec3f{0, 1, 0}) == 0.0f);
    CHECK(Vec3f{1, 2, 3}.dot(Vec3f{4, 5, 6}) == 32.0f);
}

TEST_CASE("Vec3 cross product") {
    auto c = Vec3f{1, 0, 0}.cross(Vec3f{0, 1, 0});
    CHECK(c == Vec3f{0, 0, 1});

    // Cross product is orthogonal to both inputs
    Vec3f a{2, 3, 4}, b{5, 6, 7};
    auto r = a.cross(b);
    CHECK(r.dot(a) == doctest::Approx(0.0f));
    CHECK(r.dot(b) == doctest::Approx(0.0f));

    // Anti-commutative
    CHECK(a.cross(b) == -(b.cross(a)));
}

TEST_CASE("Vec3 length") {
    Vec3f v{2, 3, 6};
    CHECK(v.length_sq() == 49.0f);
    CHECK(v.length() == 7.0f);
}

TEST_CASE("Vec3 normalized") {
    Vec3f v{3, 0, 4};
    auto n = v.normalized();
    CHECK(n.length() == doctest::Approx(1.0f));
    CHECK(n.x == doctest::Approx(0.6f));
    CHECK(n.y == doctest::Approx(0.0f));
    CHECK(n.z == doctest::Approx(0.8f));

    // Zero returns zero
    CHECK(Vec3f{}.normalized() == Vec3f{});
}

TEST_CASE("Vec3 distance") {
    CHECK(Vec3f{0, 0, 0}.distance(Vec3f{2, 3, 6}) == 7.0f);
}

TEST_CASE("Vec3 subscript") {
    Vec3f v{1, 2, 3};
    CHECK(v[0] == 1.0f); CHECK(v[1] == 2.0f); CHECK(v[2] == 3.0f);
    v[0] = 10.0f;
    CHECK(v.x == 10.0f);
}

TEST_CASE("Vec3 data pointer") {
    Vec3f v{1, 2, 3};
    const float* d = v.data();
    CHECK(d[0] == 1.0f); CHECK(d[1] == 2.0f); CHECK(d[2] == 3.0f);
}

TEST_CASE("Vec3 static helpers") {
    CHECK(Vec3f::zero()    == Vec3f{0, 0, 0});
    CHECK(Vec3f::one()     == Vec3f{1, 1, 1});
    CHECK(Vec3f::up()      == Vec3f{0, 1, 0});
    CHECK(Vec3f::forward() == Vec3f{0, 0, -1});
    CHECK(Vec3f::right()   == Vec3f{1, 0, 0});
}

TEST_CASE("Vec3 scalar * Vec3") {
    CHECK(2.0f * Vec3f{1, 2, 3} == Vec3f{2, 4, 6});
}
