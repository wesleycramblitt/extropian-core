#pragma once

#include <ext/ecs/entity.hpp>
#include <ext/ecs/component.hpp>
#include <ext/ecs/system.hpp>

#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <cassert>

namespace ext::ecs {

/// @brief Sparse-set ECS registry.
///
/// Owns all entities and their components. Systems read/write through this.
/// Components are stored in type-erased pools for cache-friendly iteration.
class Registry {
public:
    Registry() = default;
    ~Registry();

    // Non-copyable, movable
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry(Registry&&) noexcept;
    Registry& operator=(Registry&&) noexcept;

    /// ── Entity lifecycle ──────────────────────────────
    [[nodiscard]] Entity create();
    void destroy(Entity entity);
    [[nodiscard]] bool valid(Entity entity) const;

    /// ── Component storage ─────────────────────────────
    template <Component T, typename... Args>
    T& emplace(Entity entity, Args&&... args);

    template <Component T>
    void remove(Entity entity);

    template <Component T>
    [[nodiscard]] T* get(Entity entity);

    template <Component T>
    [[nodiscard]] const T* get(Entity entity) const;

    template <Component T>
    [[nodiscard]] bool has(Entity entity) const;

    /// ── Views (iterate entities with specific components) ──
    template <Component... Ts>
    class View;

    template <Component... Ts>
    [[nodiscard]] View<Ts...> view();

    /// ── Size ──────────────────────────────────────────
    [[nodiscard]] size_t entity_count() const { return entity_count_; }
    [[nodiscard]] size_t component_count() const { return pools_.size(); }

private:
    struct IPool {
        virtual ~IPool() = default;
        virtual void remove(Entity e) = 0;
        virtual bool has(Entity e) const = 0;
    };

    template <Component T>
    struct Pool;

    std::unordered_map<std::type_index, std::unique_ptr<IPool>> pools_;
    std::vector<Entity> entities_;
    std::vector<uint32_t> generations_;
    size_t entity_count_{0};
    size_t next_id_{1};
};

} // namespace ext::ecs
