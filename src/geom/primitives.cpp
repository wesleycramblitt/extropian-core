#include <exd/math/vec3.hpp>
#include <cmath>
#include <algorithm>

namespace exd::geom {

using math::Vec3;

// ── Geometric primitives ────────────────────────────

struct Sphere   { Vec3 center; float radius; };
struct Box      { Vec3 center; Vec3 half_extents; };
struct Capsule  { Vec3 a, b; float radius; };
struct Cylinder { Vec3 base, axis; float radius, height; };
struct Plane    { Vec3 normal; float distance; };

// ── Signed Distance Functions ────────────────────────

float sdf_sphere(const Vec3& p, const Sphere& s) {
    return (p - s.center).length() - s.radius;
}

float sdf_box(const Vec3& p, const Box& b) {
    Vec3 q = {
        std::abs(p.x - b.center.x) - b.half_extents.x,
        std::abs(p.y - b.center.y) - b.half_extents.y,
        std::abs(p.z - b.center.z) - b.half_extents.z
    };
    Vec3 d = {std::max(q.x, 0.0f), std::max(q.y, 0.0f), std::max(q.z, 0.0f)};
    return d.length() + std::min(std::max(q.x, std::max(q.y, q.z)), 0.0f);
}

float sdf_plane(const Vec3& p, const Plane& pl) {
    return p.dot(pl.normal) - pl.distance;
}

float csg_union(float d1, float d2) { return std::min(d1, d2); }
float csg_subtract(float d1, float d2) { return std::max(d1, -d2); }
float csg_intersect(float d1, float d2) { return std::max(d1, d2); }

} // namespace exd::geom
