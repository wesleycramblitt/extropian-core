#pragma once

namespace exd::ecs {

class Registry;

/// Abstract system. Each frame, the ECS runtime calls update() on registered systems
/// in dependency order. Systems read and write components via the Registry.
class ISystem {
public:
    virtual ~ISystem() = default;

    /// Called each frame. dt is seconds since last frame.
    virtual void update(Registry& registry, double dt) = 0;

    /// Optional: return the set of modes this system should be active in.
    /// Returns empty = always active. Override for mode-gated systems.
    [[nodiscard]] virtual const void* active_modes() const { return nullptr; }
};

} // namespace exd::ecs
