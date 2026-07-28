#pragma once

#include <exd/ecs/entity.hpp>

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <tuple>
#include <utility>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <iterator>
#include <string>

namespace exd::ecs {

// Forward-declare View (defined in view.hpp)
template <class... Cs>
class View;

// ────────────────────────────────────────────────────
//  Registry — sparse-set component storage
// ────────────────────────────────────────────────────

class Registry {
public:
    Registry() = default;
    ~Registry() = default;

    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;
    Registry(Registry&&) noexcept = default;
    Registry& operator=(Registry&&) noexcept = default;

    // ---- Entity lifecycle ----
    [[nodiscard]] Entity create(std::string name);
    void destroy(Entity e);
    [[nodiscard]] bool valid(Entity e) const noexcept;
    void clear();

    // ---- Introspection ----
    [[nodiscard]] std::vector<Entity> all_entities() const noexcept;
    [[nodiscard]] size_t entity_count() const noexcept;

    // ---- Component operations ----
    template <class T, class... Args>
    T& emplace(Entity e, Args&&... args);

    template <class T>
    void remove(Entity e);

    template <class T>
    [[nodiscard]] bool has(Entity e) const;

    template <class T>
    [[nodiscard]] T& get(Entity e);

    template <class T>
    [[nodiscard]] const T& get(Entity e) const;

    template <class T>
    [[nodiscard]] T* try_get(Entity e) noexcept;

    template <class T>
    [[nodiscard]] const T* try_get(Entity e) const noexcept;

    // ---- Views ----
    template <class... Cs>
    [[nodiscard]] View<Cs...> view();

    template <class... Cs>
    [[nodiscard]] View<const Cs...> view() const;

private:
    // ---- Pool base (type erasure) ----
    struct IPool {
        virtual ~IPool() = default;
        virtual void remove_entity(Entity::id_type id) = 0;
        virtual bool has_entity(Entity::id_type id) const = 0;
        virtual size_t size() const noexcept = 0;
        virtual const std::vector<Entity::id_type>& dense_entities() const noexcept = 0;
    };

    template <class T>
    struct Pool final : IPool {
        std::vector<T> dense_data_;
        std::vector<Entity::id_type> dense_entities_;
        std::vector<uint32_t> sparse_;

        void ensure_sparse(Entity::id_type id) {
            if (id >= sparse_.size()) sparse_.resize(static_cast<size_t>(id) + 1u, 0u);
        }

        template <class... Args>
        T& emplace(Entity::id_type id, Args&&... args) {
            ensure_sparse(id);
            if (sparse_[id] != 0u) {
                auto& existing = dense_data_[static_cast<size_t>(sparse_[id] - 1u)];
                existing = T(std::forward<Args>(args)...);
                return existing;
            }
            const auto idx = static_cast<uint32_t>(dense_data_.size());
            dense_data_.emplace_back(std::forward<Args>(args)...);
            dense_entities_.push_back(id);
            sparse_[id] = idx + 1u;
            return dense_data_.back();
        }

        void remove_entity(Entity::id_type id) override {
            if (id >= sparse_.size()) return;
            const auto sparse_val = sparse_[id];
            if (sparse_val == 0u) return;
            const uint32_t idx  = sparse_val - 1u;
            const uint32_t last = static_cast<uint32_t>(dense_data_.size() - 1u);
            if (idx != last) {
                dense_data_[idx] = std::move(dense_data_[last]);
                const auto moved_entity = dense_entities_[static_cast<size_t>(last)];
                dense_entities_[idx] = moved_entity;
                sparse_[moved_entity] = idx + 1u;
            }
            dense_data_.pop_back();
            dense_entities_.pop_back();
            sparse_[id] = 0u;
        }

        bool has_entity(Entity::id_type id) const override {
            return id < sparse_.size() && sparse_[id] != 0u;
        }

        size_t size() const noexcept override { return dense_data_.size(); }

        const std::vector<Entity::id_type>& dense_entities() const noexcept override {
            return dense_entities_;
        }

        T* try_get(Entity::id_type id) noexcept {
            if (!has_entity(id)) return nullptr;
            return &dense_data_[static_cast<size_t>(sparse_[id] - 1u)];
        }

        const T* try_get(Entity::id_type id) const noexcept {
            if (!has_entity(id)) return nullptr;
            return &dense_data_[static_cast<size_t>(sparse_[id] - 1u)];
        }
    };

    template <class T>
    auto* pool_ptr() noexcept {
        auto it = pools_.find(std::type_index(typeid(std::remove_const_t<T>)));
        return (it != pools_.end())
            ? static_cast<Pool<std::remove_const_t<T>>*>(it->second.get())
            : nullptr;
    }

    template <class T>
    auto* pool_ptr() const noexcept {
        auto it = pools_.find(std::type_index(typeid(std::remove_const_t<T>)));
        return (it != pools_.end())
            ? static_cast<const Pool<std::remove_const_t<T>>*>(it->second.get())
            : nullptr;
    }

    template <class T>
    auto& ensure_pool() {
        using U = std::remove_const_t<T>;
        auto key = std::type_index(typeid(U));
        auto it = pools_.find(key);
        if (it == pools_.end()) {
            auto p = std::make_unique<Pool<U>>();
            auto* raw = p.get();
            pools_.emplace(key, std::move(p));
            return *raw;
        }
        return *static_cast<Pool<U>*>(it->second.get());
    }

    std::vector<Entity::gen_type> gen_;
    std::vector<uint8_t> alive_;
    std::vector<Entity::id_type> free_ids_;
    std::vector<std::string> names_;
    std::unordered_map<std::type_index, std::unique_ptr<IPool>> pools_;

    // Grant friendship so View and its iterator can access gen_, alive_, pools_, pool_ptr().
    template <class... Cs>
    friend class View;
};

// ====================== Template implementations ======================

template <class T, class... Args>
inline T& Registry::emplace(Entity e, Args&&... args) {
    if (!valid(e)) throw std::runtime_error("Registry::emplace on invalid entity");
    auto& pool = ensure_pool<T>();
    return pool.emplace(e.id, std::forward<Args>(args)...);
}

template <class T>
inline void Registry::remove(Entity e) {
    if (!valid(e)) return;
    if (auto* p = pool_ptr<T>()) p->remove_entity(e.id);
}

template <class T>
inline bool Registry::has(Entity e) const {
    if (!valid(e)) return false;
    if (auto* p = pool_ptr<T>()) return p->has_entity(e.id);
    return false;
}

template <class T>
inline T& Registry::get(Entity e) {
    if (!valid(e)) throw std::runtime_error("Registry::get on invalid entity");
    auto* p = pool_ptr<T>();
    if (!p) throw std::runtime_error("Registry::get missing pool");
    auto* ptr = p->try_get(e.id);
    if (!ptr) throw std::runtime_error("Registry::get missing component");
    return *ptr;
}

template <class T>
inline const T& Registry::get(Entity e) const {
    if (!valid(e)) throw std::runtime_error("Registry::get(const) on invalid entity");
    auto* p = pool_ptr<T>();
    if (!p) throw std::runtime_error("Registry::get(const) missing pool");
    auto* ptr = p->try_get(e.id);
    if (!ptr) throw std::runtime_error("Registry::get(const) missing component");
    return *ptr;
}

template <class T>
inline T* Registry::try_get(Entity e) noexcept {
    if (!valid(e)) return nullptr;
    auto* p = pool_ptr<T>();
    return p ? p->try_get(e.id) : nullptr;
}

template <class T>
inline const T* Registry::try_get(Entity e) const noexcept {
    if (!valid(e)) return nullptr;
    auto* p = pool_ptr<T>();
    return p ? p->try_get(e.id) : nullptr;
}

} // namespace exd::ecs
