#include <exd/math/vec3.hpp>
#include <cmath>
#include <cfloat>

namespace exd::geom {

using math::Vec3;

// ── Primitives ──────────────────────────────────────

struct Sphere   { Vec3 center; float radius; };
struct Box      { Vec3 center; Vec3 half_extents; };
struct Capsule  { Vec3 a, b; float radius; };
struct Cylinder { Vec3 base; Vec3 axis; float radius; float height; };
struct Plane    { Vec3 normal; float distance; };

// ── Signed Distance Functions ────────────────────────

float sdf_sphere(const Vec3& p, const Sphere& s) {
    return (p - s.center).norm() - s.radius;
}

float sdf_box(const Vec3& p, const Box& b) {
    Vec3 d = {std::abs(p.x - b.center.x) - b.half_extents.x,
              std::abs(p.y - b.center.y) - b.half_extents.y,
              std::abs(p.z - b.center.z) - b.half_extents.z};
    return std::max(std::max(d.x, d.y), d.z);
}

float sdf_plane(const Vec3& p, const Plane& pl) {
    return p.dot(pl.normal) - pl.distance;
}

// ── CSG operations on SDFs ──────────────────────────

float csg_union(float d1, float d2) { return std::min(d1, d2); }
float csg_subtract(float d1, float d2) { return std::max(d1, -d2); }
float csg_intersect(float d1, float d2) { return std::max(d1, d2); }

} // namespace exd::geom
