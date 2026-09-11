#pragma once

#include <exd/ecs/entity.hpp>
#include <exd/ecs/registry.hpp>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <unordered_map>
#include <vector>

namespace exd::ecs {

/// Generic hierarchy over entities.
///
/// The SceneGraph answers exactly one question: *what is the hierarchical
/// relationship between these entities?* It knows about entity identity,
/// parent/child relationships, traversal, insertion/removal, reparenting,
/// sibling ordering, and relationship validity/lifetime (via prune()).
///
/// It deliberately knows NOTHING about rendering, UI, layout, cameras,
/// geometry, materials, physics, transforms, projection, or any domain
/// semantics. Spatial systems combine SceneGraph relationships with a
/// LocalTransform component (see transform.hpp) to derive world transforms —
/// the SceneGraph itself stays transform-free, so it is equally useful for
/// non-spatial hierarchies (model trees, CAE part trees, UI widget trees).
///
/// The SceneGraph is a pure containment structure: it does not own entities
/// and is orthogonal to the Registry. Destroyed/reused entities are handled
/// by prune() (generation-safe handles, same semantics as the ECS).
class SceneGraph {
public:
    // ── insertion / reparenting ────────────────────────────────────────
    /// Make `child` a child of `parent`, appended after existing siblings.
    /// Returns false on self-parenting or cycle creation.
    [[nodiscard]] bool attach(Entity parent, Entity child) {
        if (child == parent || is_descendant(parent, child)) return false;
        auto& child_node = nodes_[child];
        if (child_node.parent) detach_from_parent(*child_node.parent, child);
        child_node.parent = parent;
        nodes_[parent].children.push_back(child);
        return true;
    }

    /// Attach `child` at a specific sibling index (0 = first). Clamped.
    [[nodiscard]] bool attach_at(Entity parent, Entity child, size_t index) {
        if (child == parent || is_descendant(parent, child)) return false;
        auto& child_node = nodes_[child];
        if (child_node.parent) detach_from_parent(*child_node.parent, child);
        child_node.parent = parent;
        auto& siblings = nodes_[parent].children;
        index = std::min(index, siblings.size());
        siblings.insert(siblings.begin() + static_cast<std::ptrdiff_t>(index), child);
        return true;
    }

    // ── removal ────────────────────────────────────────────────────────
    /// Remove `child` from its parent; it becomes a root of the graph.
    [[nodiscard]] bool detach(Entity child) {
        const auto it = nodes_.find(child);
        if (it == nodes_.end() || !it->second.parent) return false;
        detach_from_parent(*it->second.parent, child);
        it->second.parent.reset();
        return true;
    }

    /// Remove an entity entirely from the graph. Its children become roots.
    void remove(Entity entity) {
        const auto it = nodes_.find(entity);
        if (it == nodes_.end()) return;
        if (it->second.parent) detach_from_parent(*it->second.parent, entity);
        for (const auto child : it->second.children) {
            if (const auto child_it = nodes_.find(child); child_it != nodes_.end())
                child_it->second.parent.reset();
        }
        nodes_.erase(it);
    }

    void clear() noexcept { nodes_.clear(); }

    /// Drop stale generational handles (entities destroyed in the Registry).
    /// Returns the number of removed nodes.
    size_t prune(const Registry& registry) {
        std::vector<Entity> stale;
        stale.reserve(nodes_.size());
        for (const auto& [entity, _] : nodes_)
            if (!registry.valid(entity)) stale.push_back(entity);
        for (const auto entity : stale) remove(entity);
        return stale.size();
    }

    // ── queries ────────────────────────────────────────────────────────
    [[nodiscard]] std::optional<Entity> parent(Entity child) const {
        const auto it = nodes_.find(child);
        return it == nodes_.end() ? std::nullopt : it->second.parent;
    }

    /// Children in sibling order (insertion/reorder order).
    [[nodiscard]] std::vector<Entity> children(Entity parent) const {
        const auto it = nodes_.find(parent);
        return it == nodes_.end() ? std::vector<Entity>{} : it->second.children;
    }

    /// Borrowed children vector (sibling order). Lifetime tied to this graph.
    [[nodiscard]] const std::vector<Entity>& children_ref(Entity parent) const {
        static const std::vector<Entity> kEmpty{};
        const auto it = nodes_.find(parent);
        return it == nodes_.end() ? kEmpty : it->second.children;
    }

    [[nodiscard]] bool contains(Entity entity) const noexcept {
        return nodes_.contains(entity);
    }

    [[nodiscard]] bool is_descendant(Entity entity, Entity ancestor) const {
        auto it = nodes_.find(entity);
        while (it != nodes_.end() && it->second.parent) {
            if (*it->second.parent == ancestor) return true;
            it = nodes_.find(*it->second.parent);
        }
        return false;
    }

    /// All entities with no parent (graph entry points).
    [[nodiscard]] std::vector<Entity> roots() const {
        std::vector<Entity> out;
        out.reserve(nodes_.size());
        for (const auto& [entity, node] : nodes_)
            if (!node.parent) out.push_back(entity);
        return out;
    }

    [[nodiscard]] size_t size() const noexcept { return nodes_.size(); }

    /// Depth below the root of the tree containing `entity` (0 = root).
    [[nodiscard]] size_t depth(Entity entity) const {
        size_t d = 0;
        auto it = nodes_.find(entity);
        while (it != nodes_.end() && it->second.parent) {
            ++d;
            it = nodes_.find(*it->second.parent);
        }
        return d;
    }

    // ── sibling ordering ───────────────────────────────────────────────
    /// Move `child` to `new_index` among its siblings (0 = first). Clamped.
    [[nodiscard]] bool reorder(Entity child, size_t new_index) {
        const auto it = nodes_.find(child);
        if (it == nodes_.end() || !it->second.parent) return false;
        auto& siblings = nodes_[*it->second.parent].children;
        const auto cur = std::find(siblings.begin(), siblings.end(), child);
        if (cur == siblings.end()) return false;
        const size_t cur_index = static_cast<size_t>(cur - siblings.begin());
        new_index = std::min(new_index, siblings.size() - 1u);
        if (cur_index == new_index) return true;
        siblings.erase(cur);
        siblings.insert(siblings.begin() + static_cast<std::ptrdiff_t>(new_index), child);
        return true;
    }

    /// Move `child` to immediately follow `after_sibling`.
    [[nodiscard]] bool move_after(Entity child, Entity after_sibling) {
        const auto it = nodes_.find(child);
        if (it == nodes_.end() || !it->second.parent) return false;
        auto& siblings = nodes_[*it->second.parent].children;
        const auto anchor = std::find(siblings.begin(), siblings.end(), after_sibling);
        if (anchor == siblings.end()) return false;
        const auto cur = std::find(siblings.begin(), siblings.end(), child);
        if (cur == siblings.end()) return false;
        if (cur == anchor || cur == anchor + 1) return true;
        siblings.erase(cur);
        const auto re_anchor = std::find(siblings.begin(), siblings.end(), after_sibling);
        const size_t re_index = static_cast<size_t>(re_anchor - siblings.begin()) + 1u;
        siblings.insert(siblings.begin() + static_cast<std::ptrdiff_t>(re_index), child);
        return true;
    }

    /// Hash for using Entity as an unordered_map key (generation-aware).
    struct EntityHash {
        size_t operator()(Entity entity) const noexcept {
            const auto id = static_cast<size_t>(entity.id);
            const auto gen = static_cast<size_t>(entity.gen);
            return id ^ (gen + static_cast<size_t>(0x9e3779b9u) + (id << 6u) + (id >> 2u));
        }
    };

    // ── traversal ──────────────────────────────────────────────────────
    /// Depth-first pre-order traversal of the subtree rooted at `root`.
    /// fn(entity, depth) is called for each descendant including the root.
    /// Returning false from fn aborts the entire traversal immediately.
    template <class Fn>
    void traverse(Entity root, Fn&& fn) const {
        traverse_impl(root, 0u, std::forward<Fn>(fn));
    }

    /// Depth-first pre-order traversal over every root of the graph.
    template <class Fn>
    void traverse_all(Fn&& fn) const {
        for (const auto root : roots())
            if (!traverse_impl(root, 0u, std::forward<Fn>(fn))) return;
    }

private:
    struct Node {
        std::optional<Entity> parent;
        std::vector<Entity> children;
    };

    void detach_from_parent(Entity parent, Entity child) {
        const auto it = nodes_.find(parent);
        if (it == nodes_.end()) return;
        auto& children = it->second.children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    /// Returns false if the walk was aborted by fn.
    template <class Fn>
    bool traverse_impl(Entity node, size_t depth, Fn&& fn) const {
        if (!fn(node, depth)) return false;
        const auto it = nodes_.find(node);
        if (it == nodes_.end()) return true;
        for (const auto child : it->second.children)
            if (!traverse_impl(child, depth + 1u, fn)) return false;
        return true;
    }

    std::unordered_map<Entity, Node, EntityHash> nodes_;
};

} // namespace exd::ecs
