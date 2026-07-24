#pragma once

#include <cstdint>
#include <type_traits>
#include <atomic>

namespace exd::ecs {

/// Global component ID counter — shared across all translation units.
inline uint32_t next_component_id() {
    static std::atomic<uint32_t> counter{0};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

/// Every component type gets a unique runtime ID via this template.
/// Components are POD or trivially-movable structs with no inheritance requirement.
template <typename T>
struct ComponentTraits {
    static uint32_t id() {
        static uint32_t s_id = next_component_id();
        return s_id;
    }
};

/// Concept: a type is a valid component if it is trivially movable and not a pointer.
template <typename T>
concept Component = std::is_trivially_move_constructible_v<T>
                 && std::is_trivially_destructible_v<T>
                 && !std::is_pointer_v<T>;

} // namespace exd::ecs
