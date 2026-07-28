#pragma once

#include <exd/ecs/registry.hpp>
#include <limits>
#include <type_traits>
#include <functional>

namespace exd::ecs {

// ────────────────────────────────────────────────────
//  View<Cs...> — iterate entities with components
// ────────────────────────────────────────────────────
//  Defined here (not nested in Registry) per the plan
//  file layout.  Friended by Registry so it can access
//  gen_, alive_, and pool_ptr().

template <class... Cs>
class View {
public:
    static_assert(sizeof...(Cs) > 0, "View requires at least one component type.");

    using registry_type = std::conditional_t<
        (std::is_const_v<Cs> || ...),
        const Registry,
        Registry
    >;

    explicit View(registry_type& r) : reg_(r), driving_(smallest_pool()) {}

    // ── Iterator ─────────────────────────────────────

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Entity;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const Entity*;
        using reference         = Entity;

        iterator() = default;
        iterator(registry_type* reg, const Registry::IPool* driving,
                 const std::vector<Entity::id_type>* ids, size_t i)
            : reg_(reg), driving_(driving), ids_(ids), i_(i) { satisfy(); }

        reference operator*() const {
            const auto id = (*ids_)[i_];
            return Entity{id, (*reg_).gen_[id]};
        }

        iterator& operator++() { ++i_; satisfy(); return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator==(const iterator& a, const iterator& b) {
            return a.ids_ == b.ids_ && a.i_ == b.i_;
        }
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }

    private:
        registry_type* reg_{nullptr};
        const Registry::IPool* driving_{nullptr};
        const std::vector<Entity::id_type>* ids_{nullptr};
        size_t i_{0};

        void satisfy() {
            if (!reg_ || !driving_ || !ids_) return;
            while (i_ < ids_->size()) {
                const auto id = (*ids_)[i_];
                Entity e{id, (*reg_).gen_[id]};
                if (!(*reg_).alive_.empty() && id < (*reg_).alive_.size() && !(*reg_).alive_[id]) { ++i_; continue; }
                if (!(*reg_).valid(e)) { ++i_; continue; }
                if (!((*reg_).template has<std::remove_const_t<Cs>>(e) && ...)) { ++i_; continue; }
                return;
            }
        }
    };

    iterator begin() {
        if (!driving_) return end();
        const auto& ids = driving_->dense_entities();
        return iterator(&reg_, driving_, &ids, 0);
    }
    iterator end() {
        if (!driving_) return iterator(&reg_, nullptr, nullptr, 0);
        const auto& ids = driving_->dense_entities();
        return iterator(&reg_, driving_, &ids, ids.size());
    }
    iterator begin() const {
        if (!driving_) return end();
        const auto& ids = driving_->dense_entities();
        return iterator(const_cast<registry_type*>(&reg_), driving_, &ids, 0);
    }
    iterator end() const {
        if (!driving_) return iterator(const_cast<registry_type*>(&reg_), nullptr, nullptr, 0);
        const auto& ids = driving_->dense_entities();
        return iterator(const_cast<registry_type*>(&reg_), driving_, &ids, ids.size());
    }

    template <class Fn>
    void each(Fn&& fn) {
        for (Entity e : *this) std::invoke(std::forward<Fn>(fn), e, fetch<Cs>(e)...);
    }

private:
    registry_type& reg_;
    const Registry::IPool* driving_{nullptr};

    const Registry::IPool* smallest_pool() const {
        const Registry::IPool* best = nullptr;
        size_t best_size = std::numeric_limits<size_t>::max();
        auto check = [&](auto* p) -> bool {
            if (!p) return false;
            if (p->size() < best_size) { best_size = p->size(); best = p; }
            return true;
        };
        const bool ok = (check(reg_.template pool_ptr<std::remove_const_t<Cs>>()) && ...);
        return ok ? best : nullptr;
    }

    template <class C>
    decltype(auto) fetch(Entity e) {
        using Base = std::remove_const_t<C>;
        if constexpr (std::is_const_v<registry_type>)
            return static_cast<const Registry&>(reg_).template get<Base>(e);
        else
            return reg_.template get<Base>(e);
    }
};

// ── Registry::view() definitions ──
// These live here so that View is a complete type.

template <class... Cs>
inline auto Registry::view() -> View<Cs...> { return View<Cs...>(*this); }

template <class... Cs>
inline auto Registry::view() const -> View<const Cs...> { return View<const Cs...>(*this); }

} // namespace exd::ecs
