#pragma once

#include <cstdint>
#include <string_view>
#include <string>

namespace ext::ecs {

/// Entity handle: an opaque 64-bit value combining id and generation.
/// Generation prevents use-after-free: incrementing generation invalidates stale handles.
struct Entity {
    uint32_t id{0};
    uint32_t generation{0};

    bool operator==(const Entity& o) const = default;
    bool operator!=(const Entity& o) const = default;

    explicit operator bool() const { return id != 0; }

    /// Combine into a single 64-bit key for use as a map key.
    [[nodiscard]] uint64_t key() const {
        return (static_cast<uint64_t>(generation) << 32) | id;
    }

    static constexpr Entity null() { return {0, 0}; }
};

} // namespace ext::ecs

namespace std {
    template<> struct hash<ext::ecs::Entity> {
        size_t operator()(const ext::ecs::Entity& e) const { return e.key(); }
    };
}
