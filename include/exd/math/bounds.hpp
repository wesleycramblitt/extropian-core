#pragma once

#include <exd/math/vec3.hpp>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdint>

namespace exd::math {

// ────────────────────────────────────────────────────
//  AABB (axis-aligned bounding box)
// ────────────────────────────────────────────────────

struct Bounds3 {
    Vec3f min{};
    Vec3f max{};

    /// Default is an inverted/invalid bounds (min > max).
    static Bounds3 empty() { return {{ INFINITY,  INFINITY,  INFINITY},
                                     {-INFINITY, -INFINITY, -INFINITY}}; }

    [[nodiscard]] bool valid() const {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    [[nodiscard]] Vec3f center() const {
        return {(min.x + max.x) * 0.5f,
                (min.y + max.y) * 0.5f,
                (min.z + max.z) * 0.5f};
    }

    [[nodiscard]] Vec3f extents() const {
        return {(max.x - min.x) * 0.5f,
                (max.y - min.y) * 0.5f,
                (max.z - min.z) * 0.5f};
    }

    [[nodiscard]] Vec3f size() const { return max - min; }

    [[nodiscard]] float volume() const {
        if (!valid()) return 0.0f;
        Vec3f s = size();
        return s.x * s.y * s.z;
    }

    /// Expand to include a point.
    void extend(const Vec3f& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }

    /// Union of two bounding boxes.
    [[nodiscard]] static Bounds3 merge(const Bounds3& a, const Bounds3& b) {
        if (!a.valid()) return b;
        if (!b.valid()) return a;
        return {Vec3f{std::min(a.min.x, b.min.x),
                      std::min(a.min.y, b.min.y),
                      std::min(a.min.z, b.min.z)},
                Vec3f{std::max(a.max.x, b.max.x),
                      std::max(a.max.y, b.max.y),
                      std::max(a.max.z, b.max.z)}};
    }

    /// Does this bounds contain a point?
    [[nodiscard]] bool contains(const Vec3f& p) const {
        return p.x >= min.x && p.x <= max.x
            && p.y >= min.y && p.y <= max.y
            && p.z >= min.z && p.z <= max.z;
    }

    /// Compute the bounding box of a set of points.
    template <typename Iter>
    [[nodiscard]] static Bounds3 from_points(Iter begin, Iter end) {
        Bounds3 b = empty();
        for (auto it = begin; it != end; ++it) b.extend(*it);
        return b;
    }

    /// Compute the bounding box of vertex positions in a MeshData.
    /// Accepts any struct with a `.position` member of type Vec3f.
    template <typename VertexT>
    [[nodiscard]] static Bounds3 from_vertices(const std::vector<VertexT>& verts) {
        Bounds3 b = empty();
        for (const auto& v : verts) b.extend(v.position);
        return b;
    }
};

} // namespace exd::math
