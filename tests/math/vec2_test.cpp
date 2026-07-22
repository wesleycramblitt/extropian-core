#include <doctest/doctest.h>
#include <exd/math/vec2.hpp>

using namespace exd::math;

TEST_CASE("Vec2 default construction") {
    Vec2f v;
    CHECK(v.x == 0.0f);
    CHECK(v.y == 0.0f);
}

TEST_CASE("Vec2 value construction") {
    Vec2f v{3.0f, 4.0f};
    CHECK(v.x == 3.0f);
    CHECK(v.y == 4.0f);
}

TEST_CASE("Vec2 addition") {
    Vec2f a{1, 2}, b{3, 4};
    auto c = a + b;
    CHECK(c.x == 4.0f);
    CHECK(c.y == 6.0f);
}

TEST_CASE("Vec2 subtraction") {
    Vec2f a{5, 7}, b{2, 3};
    auto c = a - b;
    CHECK(c.x == 3.0f);
    CHECK(c.y == 4.0f);
}

TEST_CASE("Vec2 scalar multiplication") {
    Vec2f v{2, 3};
    auto r = v * 3.0f;
    CHECK(r.x == 6.0f);
    CHECK(r.y == 9.0f);
}

TEST_CASE("Vec2 scalar division") {
    Vec2f v{6, 9};
    auto r = v / 3.0f;
    CHECK(r.x == 2.0f);
    CHECK(r.y == 3.0f);
}

TEST_CASE("Vec2 unary negation") {
    Vec2f v{1, -2};
    auto r = -v;
    CHECK(r.x == -1.0f);
    CHECK(r.y == 2.0f);
}

TEST_CASE("Vec2 compound assignment") {
    Vec2f v{1, 2};
    v += Vec2f{3, 4};
    CHECK(v == Vec2f{4, 6});

    v -= Vec2f{1, 1};
    CHECK(v == Vec2f{3, 5});

    v *= 2.0f;
    CHECK(v == Vec2f{6, 10});

    v /= 2.0f;
    CHECK(v == Vec2f{3, 5});
}

TEST_CASE("Vec2 equality") {
    CHECK(Vec2f{1, 2} == Vec2f{1, 2});
    CHECK_FALSE(Vec2f{1, 2} == Vec2f{1, 3});
    CHECK_FALSE(Vec2f{1, 2} == Vec2f{2, 2});
}

TEST_CASE("Vec2 dot product") {
    Vec2f a{1, 2}, b{3, 4};
    CHECK(a.dot(b) == 11.0f);
    // Orthogonal vectors
    CHECK(Vec2f{1, 0}.dot(Vec2f{0, 1}) == 0.0f);
}

TEST_CASE("Vec2 cross product") {
    Vec2f a{1, 0}, b{0, 1};
    CHECK(a.cross(b) == 1.0f);
    CHECK(b.cross(a) == -1.0f);
    CHECK(Vec2f{2, 3}.cross(Vec2f{4, 5}) == -2.0f);
}

TEST_CASE("Vec2 length") {
    Vec2f v{3, 4};
    CHECK(v.length_sq() == 25.0f);
    CHECK(v.length() == 5.0f);
}

TEST_CASE("Vec2 normalized") {
    Vec2f v{3, 4};
    auto n = v.normalized();
    CHECK(n.length() == doctest::Approx(1.0f));
    CHECK(n.x == doctest::Approx(0.6f));
    CHECK(n.y == doctest::Approx(0.8f));

    // Zero vector
    auto z = Vec2f{}.normalized();
    CHECK(z.x == 0.0f);
    CHECK(z.y == 0.0f);
}

TEST_CASE("Vec2 distance") {
    Vec2f a{0, 0}, b{3, 4};
    CHECK(a.distance(b) == 5.0f);
    CHECK(b.distance(a) == 5.0f);
}

TEST_CASE("Vec2 subscript") {
    Vec2f v{1, 2};
    CHECK(v[0] == 1.0f);
    CHECK(v[1] == 2.0f);
    v[0] = 5.0f;
    CHECK(v.x == 5.0f);
}

TEST_CASE("Vec2 data pointer") {
    Vec2f v{1, 2};
    const float* d = v.data();
    CHECK(d[0] == 1.0f);
    CHECK(d[1] == 2.0f);
}

TEST_CASE("Vec2 static helpers") {
    auto z = Vec2f::zero();
    CHECK(z.x == 0.0f); CHECK(z.y == 0.0f);

    auto o = Vec2f::one();
    CHECK(o.x == 1.0f); CHECK(o.y == 1.0f);

    auto up = Vec2f::up();
    CHECK(up.x == 0.0f); CHECK(up.y == 1.0f);

    auto right = Vec2f::right();
    CHECK(right.x == 1.0f); CHECK(right.y == 0.0f);
}

TEST_CASE("Vec2 scalar * Vec2") {
    Vec2f v{2, 3};
    auto r = 3.0f * v;
    CHECK(r.x == 6.0f);
    CHECK(r.y == 9.0f);
}

TEST_CASE("Vec2 integer specialization") {
    Vec2i a{1, 2}, b{3, 4};
    CHECK((a + b) == Vec2i{4, 6});
    CHECK(a.dot(b) == 11);
}
