#pragma once

#include <cstdint>
#include <type_traits>

namespace exd::ecs {

/// Every component type gets a unique runtime ID via this template.
/// Components are POD or trivially-movable structs with no inheritance requirement.
template <typename T>
struct ComponentTraits {
    static uint32_t id() {
        static uint32_t s_id = next_id();
        return s_id;
    }

private:
    static uint32_t next_id() {
        static uint32_t counter = 0;
        return counter++;
    }
};

/// Concept: a type is a valid component if it is trivially movable and not a pointer.
template <typename T>
concept Component = std::is_trivially_move_constructible_v<T>
                 && std::is_trivially_destructible_v<T>
                 && !std::is_pointer_v<T>;

} // namespace exd::ecs
