#pragma once

#include <exd/ecs/entity.hpp>
#include <exd/ecs/registry.hpp>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <unordered_map>
#include <vector>

namespace exd::ecs {

/// Runtime containment index, independent of graph relations and rendering.
/// Every relationship stores complete generational Entity handles.
class HierarchyIndex {
public:
    [[nodiscard]] bool set_parent(Entity child, Entity parent) {
        if (child == parent || is_descendant(parent, child)) return false;
        auto& child_node = nodes_[child];
        if (child_node.parent) detach(*child_node.parent, child);
        child_node.parent = parent;
        nodes_[parent].children.push_back(child);
        return true;
    }

    [[nodiscard]] bool clear_parent(Entity child) {
        const auto it = nodes_.find(child);
        if (it == nodes_.end() || !it->second.parent) return false;
        detach(*it->second.parent, child);
        it->second.parent.reset();
        return true;
    }

    /// Remove an entity from the index. Its children become roots.
    void remove(Entity entity) {
        const auto it = nodes_.find(entity);
        if (it == nodes_.end()) return;
        if (it->second.parent) detach(*it->second.parent, entity);
        for (const auto child : it->second.children) {
            if (const auto child_it = nodes_.find(child); child_it != nodes_.end())
                child_it->second.parent.reset();
        }
        nodes_.erase(it);
    }

    void clear() noexcept { nodes_.clear(); }

    /// Remove stale generational handles after Registry destruction/reuse.
    size_t prune(const Registry& registry) {
        std::vector<Entity> stale;
        stale.reserve(nodes_.size());
        for (const auto& [entity, _] : nodes_)
            if (!registry.valid(entity)) stale.push_back(entity);
        for (const auto entity : stale) remove(entity);
        return stale.size();
    }

    [[nodiscard]] std::optional<Entity> parent(Entity child) const {
        const auto it = nodes_.find(child);
        return it == nodes_.end() ? std::nullopt : it->second.parent;
    }

    [[nodiscard]] std::vector<Entity> children(Entity parent) const {
        const auto it = nodes_.find(parent);
        return it == nodes_.end() ? std::vector<Entity>{} : it->second.children;
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

private:
    struct EntityHash {
        size_t operator()(Entity entity) const noexcept {
            const auto id = static_cast<size_t>(entity.id);
            const auto gen = static_cast<size_t>(entity.gen);
            return id ^ (gen + static_cast<size_t>(0x9e3779b9u) + (id << 6u) + (id >> 2u));
        }
    };

    struct Node {
        std::optional<Entity> parent;
        std::vector<Entity> children;
    };

    void detach(Entity parent, Entity child) {
        const auto it = nodes_.find(parent);
        if (it == nodes_.end()) return;
        auto& children = it->second.children;
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    std::unordered_map<Entity, Node, EntityHash> nodes_;
};

} // namespace exd::ecs
