#pragma once

#include <cstdint>
#include <cstddef>

namespace exd::core {

// ────────────────────────────────────────────────────
//  LinearAllocator — bump allocator (arena)
// ────────────────────────────────────────────────────
//  Per-frame temporary allocations with O(1) reset.
//  Returns nullptr on overflow (no exceptions).

class LinearAllocator {
public:
    explicit LinearAllocator(size_t capacity);
    ~LinearAllocator();

    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;
    LinearAllocator(LinearAllocator&&) = delete;
    LinearAllocator& operator=(LinearAllocator&&) = delete;

    /// Allocate `size` bytes with default 8-byte alignment.
    /// Returns nullptr if capacity would be exceeded.
    [[nodiscard]] void* allocate(size_t size, size_t alignment = 8);

    /// Reset the bump pointer to zero, freeing all allocations.
    void reset();

    /// Bytes currently consumed.
    [[nodiscard]] size_t used() const;

private:
    uint8_t* buffer_   = nullptr;
    size_t   capacity_ = 0;
    size_t   offset_   = 0;
};

} // namespace exd::core
