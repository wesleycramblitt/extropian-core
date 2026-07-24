#pragma once

#include <cstdint>
#include <string_view>
#include <functional>

namespace exd::core {

// ────────────────────────────────────────────────────
//  Hash utilities
// ────────────────────────────────────────────────────

/// FNV-1a 64-bit hash for a byte range.
[[nodiscard]] inline uint64_t fnv1a_64(const void* data, size_t len) {
    const uint64_t prime = 0x100000001b3ULL;
    const uint64_t basis = 0xcbf29ce484222325ULL;
    uint64_t h = basis;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<const uint8_t*>(data)[i];
        h *= prime;
    }
    return h;
}

/// FNV-1a 64-bit hash for a string view.
[[nodiscard]] inline uint64_t fnv1a_64_str(std::string_view sv) {
    return fnv1a_64(sv.data(), sv.size());
}

/// FNV-1a 32-bit hash for a byte range.
[[nodiscard]] inline uint32_t fnv1a_32(const void* data, size_t len) {
    const uint32_t prime = 0x01000193u;
    const uint32_t basis = 0x811c9dc5u;
    uint32_t h = basis;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<const uint8_t*>(data)[i];
        h *= prime;
    }
    return h;
}

/// FNV-1a 32-bit hash for a string view.
[[nodiscard]] inline uint32_t fnv1a_32_str(std::string_view sv) {
    return fnv1a_32(sv.data(), sv.size());
}

/// Combine two hash values (boost-style hash_combine).
template <typename T>
inline void hash_combine(size_t& seed, const T& val) {
    seed ^= std::hash<T>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

/// Combine an arbitrary number of values into a single hash.
template <typename... Args>
[[nodiscard]] size_t hash_all(const Args&... args) {
    size_t seed = 0;
    (hash_combine(seed, args), ...);
    return seed;
}

/// Trivial hash for a flat type (reinterpret bytes as hash).
template <typename T>
[[nodiscard]] uint64_t hash_bytes(const T& val) {
    return fnv1a_64(&val, sizeof(T));
}

} // namespace exd::core
