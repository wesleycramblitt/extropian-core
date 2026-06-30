#include <exd/math/vec3.hpp>
#include <cfloat>
#include <algorithm>
#include <cmath>

namespace exd::geom {

using math::Vec3f;

// ── Axis-Aligned Bounding Box ────────────────────────
struct AABB {
    Vec3f min{FLT_MAX, FLT_MAX, FLT_MAX};
    Vec3f max{-FLT_MAX, -FLT_MAX, -FLT_MAX};

    void expand(const Vec3f& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }

    Vec3f center() const {
        return {0.5f*(min.x+max.x), 0.5f*(min.y+max.y), 0.5f*(min.z+max.z)};
    }
    Vec3f size() const { return {max.x-min.x, max.y-min.y, max.z-min.z}; }
    bool contains(const Vec3f& p) const {
        return p.x >= min.x && p.x <= max.x
            && p.y >= min.y && p.y <= max.y
            && p.z >= min.z && p.z <= max.z;
    }
};

// ── Ray ─────────────────────────────────────────────
struct Ray {
    Vec3f origin;
    Vec3f direction;
};

// ── Ray-AABB intersection (slab method) ────────────
static bool ray_aabb_intersect(const Ray& ray, const AABB& box, float& t_min_o, float& t_max_o) {
    float t_min = 0.0f, t_max = FLT_MAX;
    for (int i = 0; i < 3; ++i) {
        float inv_d = 1.0f / (&ray.direction.x)[i];
        float t0 = ((&box.min.x)[i] - (&ray.origin.x)[i]) * inv_d;
        float t1 = ((&box.max.x)[i] - (&ray.origin.x)[i]) * inv_d;
        if (inv_d < 0.0f) {
            float tmp = t0; t0 = t1; t1 = tmp;
        }
        if (t0 > t_min) t_min = t0;
        if (t1 < t_max) t_max = t1;
        if (t_max < t_min) return false;
    }
    t_min_o = t_min;
    t_max_o = t_max;
    return true;
}

} // namespace exd::geom
