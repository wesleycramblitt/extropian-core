#pragma once

#include <exd/ecs/registry.hpp>
#include <exd/ecs/system.hpp>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

namespace exd::ecs {

// ────────────────────────────────────────────────────
//  SystemGraph — owns and runs systems in order
// ────────────────────────────────────────────────────

/// Manages a collection of ISystem instances and runs
/// them each frame. Systems are executed in the order
/// they are added (first-added = first-run).

class SystemGraph {
public:
    SystemGraph() = default;

    /// Add a system.  Takes ownership.  Returns index for later access.
    /// Systems run in the order they are added.
    template <typename T, typename... Args>
    requires std::derived_from<T, ISystem>
    T& add(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *ptr;
        systems_.push_back(std::move(ptr));
        return ref;
    }

    /// Add a pre-existing system (no ownership).  Caller must keep it alive.
    void add_ref(ISystem* sys) {
        refs_.push_back(sys);
    }

    /// Run all systems in order.
    void update(Registry& registry, double dt) {
        for (auto& s : systems_) s->update(registry, dt);
        for (auto* s : refs_)     s->update(registry, dt);
    }

    /// Remove all owned systems.
    void clear() { systems_.clear(); refs_.clear(); }

    [[nodiscard]] size_t count() const noexcept {
        return systems_.size() + refs_.size();
    }

private:
    std::vector<std::unique_ptr<ISystem>> systems_;
    std::vector<ISystem*> refs_;
};

} // namespace exd::ecs
