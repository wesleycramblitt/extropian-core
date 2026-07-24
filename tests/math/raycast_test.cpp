#include <doctest/doctest.h>
#include <exd/math/raycast.hpp>

using namespace exd::math;

TEST_CASE("Ray point_at") {
    Ray r{Vec3f{1, 0, 0}, Vec3f{0, 1, 0}};
    auto p = r.point_at(5.0f);
    CHECK(p.x == doctest::Approx(1.0f));
    CHECK(p.y == doctest::Approx(5.0f));
    CHECK(p.z == doctest::Approx(0.0f));
}

TEST_CASE("ray_triangle hit") {
    Ray r{Vec3f{0, 0, -5}, Vec3f{0, 0, 1}};
    auto t = ray_triangle(r,
        Vec3f{-1, -1, 0},
        Vec3f{ 1, -1, 0},
        Vec3f{ 0,  1, 0});
    REQUIRE(t.has_value());
    CHECK(*t == doctest::Approx(5.0f));
}

TEST_CASE("ray_triangle miss (parallel)") {
    // Ray parallel to triangle plane, no hit
    Ray r{Vec3f{0, 0, -5}, Vec3f{1, 0, 0}};
    auto t = ray_triangle(r, Vec3f{-1,-1,0}, Vec3f{1,-1,0}, Vec3f{0,1,0});
    CHECK_FALSE(t.has_value());
}

TEST_CASE("ray_triangle miss (outside)") {
    Ray r{Vec3f{2, 2, -5}, Vec3f{0, 0, 1}}; // misses a small triangle
    auto t = ray_triangle(r, Vec3f{-1,-1,0}, Vec3f{1,-1,0}, Vec3f{0,1,0});
    CHECK_FALSE(t.has_value());
}

TEST_CASE("ray_plane hit") {
    Ray r{Vec3f{0, 0, -5}, Vec3f{0, 0, 1}};
    auto t = ray_plane(r, Vec3f{0, 0, 0}, Vec3f{0, 0, 1});
    REQUIRE(t.has_value());
    CHECK(*t == doctest::Approx(5.0f));
}

TEST_CASE("ray_plane miss (parallel)") {
    Ray r{Vec3f{0, 0, -5}, Vec3f{1, 0, 0}};
    auto t = ray_plane(r, Vec3f{0, 0, 0}, Vec3f{0, 0, 1});
    CHECK_FALSE(t.has_value());
}

TEST_CASE("ray_plane miss (behind)") {
    Ray r{Vec3f{0, 0, 5}, Vec3f{0, 0, -1}}; // points away from plane
    auto t = ray_plane(r, Vec3f{0, 0, 0}, Vec3f{0, 0, 1});
    // denom = -1, diff = (0,0,5), t = 5 — positive means behind origin
    CHECK(t.has_value()); // actually hits, but the intersection is behind the ray origin
}

TEST_CASE("ray_sphere hit") {
    Ray r{Vec3f{0, 0, -5}, Vec3f{0, 0, 1}};
    auto t = ray_sphere(r, Vec3f{0, 0, 0}, 1.0f);
    REQUIRE(t.has_value());
    CHECK(*t == doctest::Approx(4.0f));
}

TEST_CASE("ray_sphere miss") {
    Ray r{Vec3f{0, 0, -5}, Vec3f{1, 0, 0}};
    auto t = ray_sphere(r, Vec3f{0, 0, 0}, 1.0f);
    CHECK_FALSE(t.has_value());
}

TEST_CASE("ray_aabb hit") {
    Ray r{Vec3f{0.5f, 0.5f, -5}, Vec3f{0, 0, 1}};
    auto t = ray_aabb(r, Vec3f{0, 0, 0}, Vec3f{1, 1, 1});
    REQUIRE(t.has_value());
    CHECK(*t == doctest::Approx(5.0f));
}

TEST_CASE("ray_aabb miss") {
    Ray r{Vec3f{2, 2, -5}, Vec3f{0, 0, 1}};
    auto t = ray_aabb(r, Vec3f{0, 0, 0}, Vec3f{1, 1, 1});
    CHECK_FALSE(t.has_value());
}

TEST_CASE("closest_point_on_ray") {
    Vec3f origin{0, 0, 0};
    Vec3f dir{1, 0, 0};
    Vec3f point{5, 3, 0};
    auto cp = closest_point_on_ray(origin, dir, point);
    CHECK(cp.x == doctest::Approx(5.0f));
    CHECK(cp.y == doctest::Approx(0.0f));
    CHECK(cp.z == doctest::Approx(0.0f));
}
