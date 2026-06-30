#include <exd/math/vec3.hpp>
#include <cmath>
#include <algorithm>

namespace exd::geom {

using math::Vec3f;

// ── Geometric primitives ────────────────────────────

struct Sphere   { Vec3f center; float radius; };
struct Box      { Vec3f center; Vec3f half_extents; };
struct Capsule  { Vec3f a, b; float radius; };
struct Cylinder { Vec3f base, axis; float radius, height; };
struct Plane    { Vec3f normal; float distance; };

// ── Signed Distance Functions ────────────────────────

static float sdf_sphere(const Vec3f& p, const Sphere& s) {
    return (p - s.center).length() - s.radius;
}

static float sdf_box(const Vec3f& p, const Box& b) {
    float qx = std::abs(p.x - b.center.x) - b.half_extents.x;
    float qy = std::abs(p.y - b.center.y) - b.half_extents.y;
    float qz = std::abs(p.z - b.center.z) - b.half_extents.z;
    float dx = std::max(qx, 0.0f);
    float dy = std::max(qy, 0.0f);
    float dz = std::max(qz, 0.0f);
    float d_len = std::sqrt(dx*dx + dy*dy + dz*dz);
    float m = std::min(std::max(qx, std::max(qy, qz)), 0.0f);
    return d_len + m;
}

static float sdf_plane(const Vec3f& p, const Plane& pl) {
    return p.dot(pl.normal) - pl.distance;
}

float csg_union(float d1, float d2) { return std::min(d1, d2); }
float csg_subtract(float d1, float d2) { return std::max(d1, -d2); }
float csg_intersect(float d1, float d2) { return std::max(d1, d2); }

} // namespace exd::geom
