#include <exd/math/vec3.hpp>
#include <cmath>

namespace exd::geom {
namespace {

using vec3f = exd::math::Vec3f;

inline float maxf(float a, float b) { return a > b ? a : b; }
inline float minf(float a, float b) { return a < b ? a : b; }

struct Sphere   { vec3f center; float radius; };
struct Box      { vec3f center, half_extents; };
struct Plane    { vec3f normal; float distance; };

float sdf_sphere(const vec3f& p, const Sphere& s) {
    return (p - s.center).length() - s.radius;
}

float sdf_box(const vec3f& p, const Box& b) {
    float qx = std::abs(p.x - b.center.x) - b.half_extents.x;
    float qy = std::abs(p.y - b.center.y) - b.half_extents.y;
    float qz = std::abs(p.z - b.center.z) - b.half_extents.z;
    return std::sqrt(maxf(qx,0)*maxf(qx,0) + maxf(qy,0)*maxf(qy,0) + maxf(qz,0)*maxf(qz,0))
           + minf(maxf(qx, maxf(qy, qz)), 0.0f);
}

float sdf_plane(const vec3f& p, const Plane& pl) {
    return p.dot(pl.normal) - pl.distance;
}

} // anon namespace

float csg_union(float d1, float d2) { return minf(d1, d2); }
float csg_subtract(float d1, float d2) { return maxf(d1, -d2); }
float csg_intersect(float d1, float d2) { return maxf(d1, d2); }

} // namespace exd::geom
