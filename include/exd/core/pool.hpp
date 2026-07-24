#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <cstdlib>
#include <new>

namespace exd::core {

// ────────────────────────────────────────────────────
//  Pool<T> — fixed-size slot allocator
// ────────────────────────────────────────────────────
//  O(1) allocate / deallocate.  Uses a free-list for
//  recycling.  Ideal for component pools, particle systems,
//  and any workload with many same-size objects.

template <typename T, size_t ChunkSize = 4096>
class Pool {
public:
    using value_type = T;

    Pool()  = default;
    ~Pool() { clear(); }

    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool(Pool&&) noexcept = default;
    Pool& operator=(Pool&&) noexcept = default;

    /// Allocate a slot.  Returns nullptr if full.
    [[nodiscard]] T* allocate() {
        if (free_head_) {
            // Pop from free list
            Slot* slot = free_head_;
            free_head_ = slot->next;
            return &slot->data;
        }
        // Allocate a new chunk if needed
        if (chunks_.empty() || chunk_cursor_ >= ChunkSize) {
            chunks_.push_back(static_cast<Slot*>(std::malloc(sizeof(Slot) * ChunkSize)));
            chunk_cursor_ = 0;
        }
        return &chunks_.back()[chunk_cursor_++].data;
    }

    /// Release a slot previously returned by allocate().
    void deallocate(T* p) {
        if (!p) return;
        Slot* slot = reinterpret_cast<Slot*>(p);
        slot->next = free_head_;
        free_head_ = slot;
    }

    /// Release all memory.
    void clear() {
        for (auto* chunk : chunks_) std::free(chunk);
        chunks_.clear();
        chunk_cursor_ = 0;
        free_head_ = nullptr;
    }

private:
    union Slot {
        T     data;
        Slot* next;
        Slot() {}
        ~Slot() {}
    };

    std::vector<Slot*> chunks_;
    size_t chunk_cursor_ = 0;
    Slot* free_head_ = nullptr;
};

} // namespace exd::core
