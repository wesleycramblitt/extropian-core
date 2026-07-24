#include <doctest/doctest.h>
#include <exd/math/bounds.hpp>

using namespace exd::math;

TEST_CASE("Bounds3 empty") {
    auto b = Bounds3::empty();
    CHECK_FALSE(b.valid());
    CHECK(b.volume() == doctest::Approx(0.0f));
}

TEST_CASE("Bounds3 extend") {
    auto b = Bounds3::empty();
    b.extend(Vec3f{1, 2, 3});
    CHECK(b.valid());
    CHECK(b.min == Vec3f{1, 2, 3});
    CHECK(b.max == Vec3f{1, 2, 3});

    b.extend(Vec3f{-1, 5, 0});
    CHECK(b.min == Vec3f{-1, 2, 0});
    CHECK(b.max == Vec3f{1, 5, 3});
}

TEST_CASE("Bounds3 center") {
    Bounds3 b{Vec3f{0, 0, 0}, Vec3f{2, 4, 6}};
    auto c = b.center();
    CHECK(c == Vec3f{1, 2, 3});
}

TEST_CASE("Bounds3 extents and size") {
    Bounds3 b{Vec3f{0, 0, 0}, Vec3f{2, 4, 6}};
    auto e = b.extents();
    CHECK(e == Vec3f{1, 2, 3});
    auto s = b.size();
    CHECK(s == Vec3f{2, 4, 6});
}

TEST_CASE("Bounds3 volume") {
    Bounds3 b{Vec3f{0, 0, 0}, Vec3f{2, 3, 4}};
    CHECK(b.volume() == doctest::Approx(24.0f));
}

TEST_CASE("Bounds3 merge") {
    Bounds3 a{Vec3f{0, 0, 0}, Vec3f{2, 2, 2}};
    Bounds3 b{Vec3f{1, 1, 1}, Vec3f{3, 3, 3}};
    auto m = Bounds3::merge(a, b);
    CHECK(m.min == Vec3f{0, 0, 0});
    CHECK(m.max == Vec3f{3, 3, 3});
}

TEST_CASE("Bounds3 merge with empty") {
    Bounds3 a = Bounds3::empty();
    Bounds3 b{Vec3f{1, 2, 3}, Vec3f{4, 5, 6}};
    auto m = Bounds3::merge(a, b);
    CHECK(m.min == Vec3f{1, 2, 3});
    CHECK(m.max == Vec3f{4, 5, 6});
}

TEST_CASE("Bounds3 contains") {
    Bounds3 b{Vec3f{0, 0, 0}, Vec3f{10, 10, 10}};
    CHECK(b.contains(Vec3f{5, 5, 5}));
    CHECK(b.contains(Vec3f{0, 0, 0}));
    CHECK(b.contains(Vec3f{10, 10, 10}));
    CHECK_FALSE(b.contains(Vec3f{-1, 5, 5}));
    CHECK_FALSE(b.contains(Vec3f{5, 11, 5}));
}

TEST_CASE("Bounds3 from_points") {
    std::vector<Vec3f> pts = {{1,0,0}, {0,2,0}, {0,0,3}};
    auto b = Bounds3::from_points(pts.begin(), pts.end());
    CHECK(b.min == Vec3f{0, 0, 0});
    CHECK(b.max == Vec3f{1, 2, 3});
}

TEST_CASE("Bounds3 from_vertices") {
    struct V { Vec3f position; float dummy; };
    std::vector<V> verts = {
        {{1,0,0}, 0}, {{0,2,0}, 0}, {{0,0,3}, 0}
    };
    auto b = Bounds3::from_vertices(verts);
    CHECK(b.min == Vec3f{0, 0, 0});
    CHECK(b.max == Vec3f{1, 2, 3});
}
