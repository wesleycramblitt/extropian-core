#include <exd/math/vec3.hpp>
#include <cfloat>
#include <algorithm>
#include <cmath>

namespace exd::geom {

using math::Vec3;

// ── Axis-Aligned Bounding Box ────────────────────────
struct AABB {
    Vec3 min{FLT_MAX, FLT_MAX, FLT_MAX};
    Vec3 max{-FLT_MAX, -FLT_MAX, -FLT_MAX};

    void expand(const Vec3& p) {
        min = {std::min(min.x, p.x), std::min(min.y, p.y), std::min(min.z, p.z)};
        max = {std::max(max.x, p.x), std::max(max.y, p.y), std::max(max.z, p.z)};
    }

    Vec3 center() const { return {(min.x+max.x)*0.5f, (min.y+max.y)*0.5f, (min.z+max.z)*0.5f}; }
    Vec3 size() const { return {max.x-min.x, max.y-min.y, max.z-min.z}; }
    bool contains(const Vec3& p) const {
        return p.x >= min.x && p.x <= max.x
            && p.y >= min.y && p.y <= max.y
            && p.z >= min.z && p.z <= max.z;
    }
};

// ── Ray ─────────────────────────────────────────────
struct Ray {
    Vec3 origin;
    Vec3 direction;
};

// ── Ray-AABB intersection (slab method) ────────────
inline bool ray_aabb_intersect(const Ray& ray, const AABB& box, float& t_min, float& t_max) {
    t_min = 0.0f; t_max = FLT_MAX;
    for (int i = 0; i < 3; ++i) {
        float inv_d = 1.0f / (&ray.direction.x)[i];
        float t0 = ((&box.min.x)[i] - (&ray.origin.x)[i]) * inv_d;
        float t1 = ((&box.max.x)[i] - (&ray.origin.x)[i]) * inv_d;
        if (inv_d < 0.0f) std::swap(t0, t1);
        t_min = std::max(t_min, t0);
        t_max = std::min(t_max, t1);
        if (t_max < t_min) return false;
    }
    return true;
}

} // namespace exd::geom
