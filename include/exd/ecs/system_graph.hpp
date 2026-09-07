#pragma once

#include <exd/ecs/registry.hpp>
#include <exd/ecs/system.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace exd::ecs {

enum class SystemPhase : uint8_t {
    Input = 0,
    Structural,
    Layout,
    Interaction,
    Animation,
    RenderPreparation,
    Render,

    // Legacy names retained for source compatibility.
    Simulation = Structural,
    Presentation = RenderPreparation,
};

inline constexpr std::array<SystemPhase, 7> system_phase_order{
    SystemPhase::Input,
    SystemPhase::Structural,
    SystemPhase::Layout,
    SystemPhase::Interaction,
    SystemPhase::Animation,
    SystemPhase::RenderPreparation,
    SystemPhase::Render,
};

/// Public view of one registered system (used by debug/inspector UIs).
struct SystemView {
    size_t id = 0;
    SystemPhase phase = SystemPhase::Input;
    ISystem* system = nullptr;
    bool enabled = true;
    bool is_protected = false;   ///< cannot be disabled or deregistered
};

/// Owns and runs systems in fixed phase order. Systems in one phase run in
/// insertion order. Every registered system gets a stable, non-repeating
/// `id` usable with set_enabled()/remove() (identity-based management avoids
/// dangling raw pointers for inspector UIs).
class SystemGraph {
public:
    SystemGraph() = default;

    /// Backward-compatible add: systems use the Simulation phase.
    template <typename T, typename... Args>
    requires std::derived_from<T, ISystem>
    T& add(Args&&... args) {
        return add<T>(SystemPhase::Simulation, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    requires std::derived_from<T, ISystem>
    T& add(SystemPhase phase, Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *ptr;
        systems_.push_back({next_id_++, phase, std::move(ptr), nullptr, true});
        return ref;
    }

    /// Backward-compatible add_ref: references use the Simulation phase.
    /// Returns the stable id usable with set_enabled()/remove().
    size_t add_ref(ISystem* sys) {
        return add_ref(SystemPhase::Simulation, sys);
    }

    size_t add_ref(SystemPhase phase, ISystem* sys) {
        systems_.push_back({next_id_++, phase, nullptr, sys, true});
        return next_id_ - 1;
    }

    void update(Registry& registry, double dt) {
        for (const auto phase : system_phase_order) {
            for (auto& entry : systems_) {
                if (!entry.enabled) continue;
                if (entry.phase != phase) continue;
                if (entry.owned) entry.owned->update(registry, dt);
                else if (entry.ref) entry.ref->update(registry, dt);
            }
        }
    }

    void clear() { systems_.clear(); }

    [[nodiscard]] size_t count() const noexcept { return systems_.size(); }

    // ── Identity-based management (inspector/deregistration) ──
    [[nodiscard]] std::vector<SystemView> snapshot() const {
        std::vector<SystemView> out;
        out.reserve(systems_.size());
        for (const auto& e : systems_) {
            ISystem* sys = e.owned ? e.owned.get() : e.ref;
            out.push_back({e.id, e.phase, sys, e.enabled, e.is_protected});
        }
        return out;
    }

    [[nodiscard]] bool contains(size_t id) const {
        for (const auto& e : systems_) if (e.id == id) return true;
        return false;
    }

    void set_enabled(size_t id, bool enabled) {
        for (auto& e : systems_) if (e.id == id) {
            if (e.is_protected && !enabled) return;   // protected: cannot turn off
            e.enabled = enabled;
            return;
        }
    }

    /// Deregister a system by id (erases the entry; the owned system is freed).
    /// Protected systems refuse removal.
    void remove(size_t id) {
        for (auto it = systems_.begin(); it != systems_.end(); ++it) {
            if (it->id == id) {
                if (it->is_protected) return;
                systems_.erase(it);
                return;
            }
        }
    }

    /// Protect a system id from disable/removal (e.g. the UI host).
    void protect(size_t id, bool on = true) {
        for (auto& e : systems_) if (e.id == id) { e.is_protected = on; return; }
    }
    [[nodiscard]] bool is_protected(size_t id) const {
        for (const auto& e : systems_) if (e.id == id) return e.is_protected;
        return false;
    }

private:
    struct Entry {
        size_t id;
        SystemPhase phase;
        std::unique_ptr<ISystem> owned;
        ISystem* ref;
        bool enabled;
        bool is_protected = false;
    };

    std::vector<Entry> systems_;
    size_t next_id_ = 0;
};

} // namespace exd::ecs
