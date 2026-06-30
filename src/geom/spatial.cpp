#include <exd/math/vec3.hpp>
#include <cfloat>
#include <cmath>

namespace exd::geom {
namespace {

using vec3f = exd::math::Vec3f;

struct AABB {
    vec3f min{FLT_MAX, FLT_MAX, FLT_MAX};
    vec3f max{-FLT_MAX, -FLT_MAX, -FLT_MAX};
};

struct Ray {
    vec3f origin;
    vec3f direction;
};

bool ray_aabb_intersect(const Ray& ray, const AABB& box, float& t_min_o, float& t_max_o) {
    float tmin = 0.0f, tmax = FLT_MAX;
    for (int i = 0; i < 3; ++i) {
        float inv_d = 1.0f / (&ray.direction.x)[i];
        float t0 = ((&box.min.x)[i] - (&ray.origin.x)[i]) * inv_d;
        float t1 = ((&box.max.x)[i] - (&ray.origin.x)[i]) * inv_d;
        if (inv_d < 0.0f) { float tmp = t0; t0 = t1; t1 = tmp; }
        if (t0 > tmin) tmin = t0;
        if (t1 < tmax) tmax = t1;
        if (tmax < tmin) return false;
    }
    t_min_o = tmin;
    t_max_o = tmax;
    return true;
}

} // anon namespace
} // namespace exd::geom
