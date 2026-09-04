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

/// Owns and runs systems in fixed phase order. Systems in one phase run in
/// insertion order.
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
        systems_.push_back({phase, std::move(ptr), nullptr});
        return ref;
    }

    /// Backward-compatible add_ref: references use the Simulation phase.
    void add_ref(ISystem* sys) { add_ref(SystemPhase::Simulation, sys); }

    void add_ref(SystemPhase phase, ISystem* sys) {
        systems_.push_back({phase, nullptr, sys});
    }

    void update(Registry& registry, double dt) {
        for (const auto phase : system_phase_order) {
            for (auto& entry : systems_) {
                if (entry.phase != phase) continue;
                if (entry.owned) entry.owned->update(registry, dt);
                else if (entry.ref) entry.ref->update(registry, dt);
            }
        }
    }

    void clear() { systems_.clear(); }

    [[nodiscard]] size_t count() const noexcept { return systems_.size(); }

private:
    struct Entry {
        SystemPhase phase;
        std::unique_ptr<ISystem> owned;
        ISystem* ref;
    };

    std::vector<Entry> systems_;
};

} // namespace exd::ecs
