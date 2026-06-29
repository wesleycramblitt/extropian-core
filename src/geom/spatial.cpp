#include <exd/math/vec3.hpp>
#include <vector>
#include <array>
#include <cfloat>
#include <algorithm>
#include <cstdint>

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
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y && p.z >= min.z && p.z <= max.z;
    }
};

// ── Ray ─────────────────────────────────────────────

struct Ray {
    Vec3 origin, direction;
};

// ── Ray-AABB intersection ────────────────────────────

bool ray_aabb_intersect(const Ray& ray, const AABB& box, float& t_min, float& t_max) {
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

// ── Octree (simple fixed-depth for spatial queries) ──

struct OctreeNode {
    AABB bounds;
    std::vector<uint32_t> entity_ids;
    std::array<OctreeNode*, 8> children{};
    OctreeNode* parent = nullptr;
    int depth = 0;
    bool is_leaf = true;
};

class Octree {
    OctreeNode* root_ = nullptr;
    int max_depth_ = 6;
    int max_entities_ = 8;

public:
    void build(const AABB& bounds) {
        root_ = new OctreeNode{bounds};
    }

    void insert(uint32_t entity_id, const AABB& bounds) {
        insert_impl(root_, entity_id, bounds);
    }

private:
    void insert_impl(OctreeNode* node, uint32_t id, const AABB& bounds) {
        if (!node || !node->bounds.contains(bounds.center())) return;

        if (node->is_leaf && (int)node->entity_ids.size() < max_entities_) {
            node->entity_ids.push_back(id);
            return;
        }

        if (node->depth >= max_depth_) return;

        if (node->is_leaf) {
            Vec3 c = node->bounds.center();
            Vec3 s = node->bounds.size() * 0.5f;
            for (int i = 0; i < 8; ++i) {
                Vec3 child_min = {
                    (i & 1) ? c.x : node->bounds.min.x,
                    (i & 2) ? c.y : node->bounds.min.y,
                    (i & 4) ? c.z : node->bounds.min.z
                };
                Vec3 child_max = {child_min.x + s.x, child_min.y + s.y, child_min.z + s.z};
                node->children[i] = new OctreeNode{{child_min, child_max}, {}, {}, node, node->depth + 1};
            }
            node->is_leaf = false;
            for (auto eid : node->entity_ids) insert_impl(node, eid, bounds);
            node->entity_ids.clear();
        }

        for (int i = 0; i < 8; ++i)
            insert_impl(node->children[i], id, bounds);
    }
};

} // namespace exd::geom
