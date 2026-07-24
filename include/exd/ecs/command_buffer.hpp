#pragma once

#include <exd/ecs/registry.hpp>
#include <vector>
#include <functional>
#include <string>
#include <utility>
#include <tuple>

namespace exd::ecs {

// ────────────────────────────────────────────────────
//  CommandBuffer — deferred entity & component mutations
// ────────────────────────────────────────────────────
//  Queue up entity lifecycle and component changes safely
//  during View iteration.  Call execute() afterwards to
//  apply all commands in order.

class CommandBuffer {
public:
    using CmdFn = std::function<void(Registry&)>;

    explicit CommandBuffer(Registry& reg) : reg_(&reg) {}

    /// Schedule entity creation.  The entity is not usable
    /// until execute() returns — use Registry queries after.
    void create(std::string name) {
        commands_.push_back([name = std::move(name)](Registry& r) {
            std::ignore = r.create(std::move(name));
        });
    }

    /// Schedule entity destruction.
    void destroy(Entity e) {
        commands_.push_back([e](Registry& r) { r.destroy(e); });
    }

    /// Schedule component emplace (add or overwrite).
    /// Arguments are captured by value for deferred execution.
    template <typename T, typename... Args>
    void emplace(Entity e, Args&&... args) {
        commands_.push_back(
            [e, ...args = std::forward<Args>(args)](Registry& r) mutable {
                r.template emplace<T>(e, std::forward<Args>(args)...);
            });
    }

    /// Schedule component removal.
    template <typename T>
    void remove(Entity e) {
        commands_.push_back([e](Registry& r) { r.template remove<T>(e); });
    }

    /// Execute all queued commands against the registry, in FIFO order.
    void execute() {
        for (auto& cmd : commands_) cmd(*reg_);
        commands_.clear();
    }

    /// Discard all pending commands without executing.
    void clear() { commands_.clear(); }

    [[nodiscard]] size_t pending() const noexcept { return commands_.size(); }
    [[nodiscard]] bool   empty()   const noexcept { return commands_.empty(); }

private:
    Registry* reg_ = nullptr;
    std::vector<CmdFn> commands_;
};

} // namespace exd::ecs
