#pragma once

#include <exd/math/vec3.hpp>
#include <cfloat>
#include <optional>
#include <cmath>

namespace exd::math {

// ────────────────────────────────────────────────────
//  Ray
// ────────────────────────────────────────────────────

/// Infinite ray defined by an origin and a normalized direction.
struct Ray {
    Vec3f origin;
    Vec3f direction;   // must be unit-length

    [[nodiscard]] Vec3f point_at(float t) const {
        return {origin.x + direction.x * t,
                origin.y + direction.y * t,
                origin.z + direction.z * t};
    }
};

// ────────────────────────────────────────────────────
//  Intersection tests
// ────────────────────────────────────────────────────

/// Möller–Trumbore ray → triangle intersection.
/// Returns distance t along the ray, or nullopt for no hit.
/// v0, v1, v2 are counter-clockwise triangle vertices.
[[nodiscard]] inline std::optional<float> ray_triangle(
    const Ray& ray,
    const Vec3f& v0, const Vec3f& v1, const Vec3f& v2)
{
    const float eps = 1e-7f;
    Vec3f e1 = v1 - v0;
    Vec3f e2 = v2 - v0;

    Vec3f h = ray.direction.cross(e2);
    float a = e1.dot(h);
    if (a > -eps && a < eps) return std::nullopt;

    float f = 1.0f / a;
    Vec3f s = ray.origin - v0;
    float u = f * s.dot(h);
    if (u < 0.0f || u > 1.0f) return std::nullopt;

    Vec3f q = s.cross(e1);
    float v = f * ray.direction.dot(q);
    if (v < 0.0f || u + v > 1.0f) return std::nullopt;

    float t = f * e2.dot(q);
    return (t > eps) ? std::optional<float>{t} : std::nullopt;
}

/// Ray → plane intersection.
/// plane_normal must be unit-length.
[[nodiscard]] inline std::optional<float> ray_plane(
    const Ray& ray,
    const Vec3f& plane_point,
    const Vec3f& plane_normal)
{
    float denom = ray.direction.dot(plane_normal);
    if (std::fabs(denom) < 1e-7f) return std::nullopt;

    Vec3f diff = ray.origin - plane_point;
    float t = -diff.dot(plane_normal) / denom;
    return (t >= 0.0f) ? std::optional<float>{t} : std::nullopt;
}

/// Ray → sphere intersection. Returns nearest positive t, or nullopt.
[[nodiscard]] inline std::optional<float> ray_sphere(
    const Ray& ray,
    const Vec3f& center, float radius)
{
    Vec3f oc = ray.origin - center;
    float b = oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float disc = b * b - c;
    if (disc < 0.0f) return std::nullopt;

    float sqrt_disc = std::sqrt(disc);
    float t0 = -b - sqrt_disc;
    float t1 = -b + sqrt_disc;

    if (t0 >= 0.0f) return t0;
    if (t1 >= 0.0f) return t1;
    return std::nullopt;
}

/// Ray → AABB intersection (slab method).
/// Returns nearest positive t, or nullopt.
[[nodiscard]] inline std::optional<float> ray_aabb(
    const Ray& ray,
    const Vec3f& bmin, const Vec3f& bmax)
{
    float tmin = -FLT_MAX, tmax = FLT_MAX;
    float ox = ray.origin.x, oy = ray.origin.y, oz = ray.origin.z;
    float dx = ray.direction.x, dy = ray.direction.y, dz = ray.direction.z;

    // X slab
    if (std::fabs(dx) > 1e-7f) {
        float t1 = (bmin.x - ox) / dx;
        float t2 = (bmax.x - ox) / dx;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
    } else if (ox < bmin.x || ox > bmax.x) {
        return std::nullopt;
    }

    // Y slab
    if (std::fabs(dy) > 1e-7f) {
        float t1 = (bmin.y - oy) / dy;
        float t2 = (bmax.y - oy) / dy;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
    } else if (oy < bmin.y || oy > bmax.y) {
        return std::nullopt;
    }

    // Z slab
    if (std::fabs(dz) > 1e-7f) {
        float t1 = (bmin.z - oz) / dz;
        float t2 = (bmax.z - oz) / dz;
        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
    } else if (oz < bmin.z || oz > bmax.z) {
        return std::nullopt;
    }

    if (tmin <= tmax && tmax >= 0.0f)
        return (tmin >= 0.0f) ? tmin : tmax;
    return std::nullopt;
}

/// Closest point on a ray to a given point.
[[nodiscard]] inline Vec3f closest_point_on_ray(
    const Vec3f& ray_origin,
    const Vec3f& ray_dir,
    const Vec3f& point)
{
    float t = (point - ray_origin).dot(ray_dir);
    return ray_origin + ray_dir * t;
}

} // namespace exd::math
