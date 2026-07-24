#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

namespace exd::core {

// ────────────────────────────────────────────────────
//  Handle<T> — type-safe generational handle
// ────────────────────────────────────────────────────
//  Pattern: an opaque uint32_t index + uint32_t generation.
//  The generation prevents use-after-free: when a slot is
//  recycled, its generation is bumped, invalidating old handles.
//  T is a phantom tag type for type safety.

template <typename Tag>
struct Handle {
    using index_type = uint32_t;
    using gen_type   = uint32_t;

    index_type index = std::numeric_limits<index_type>::max();
    gen_type   gen   = 0;

    [[nodiscard]] bool valid() const noexcept {
        return index != std::numeric_limits<index_type>::max();
    }

    [[nodiscard]] bool operator==(const Handle& o) const noexcept = default;
    [[nodiscard]] bool operator!=(const Handle& o) const noexcept = default;

    /// Null/invalid handle.
    static Handle null() { return {}; }
};

} // namespace exd::core
