#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace exd::core {

/// Simple linear allocator — bump pointer within a pre-allocated block.
/// Used for temporary per-frame allocations that are discarded together.

class LinearAllocator {
    uint8_t* buffer_ = nullptr;
    size_t capacity_ = 0;
    size_t offset_ = 0;

public:
    LinearAllocator(size_t capacity) : capacity_(capacity) {
        buffer_ = static_cast<uint8_t*>(std::malloc(capacity));
    }

    ~LinearAllocator() { std::free(buffer_); }

    void* allocate(size_t size, size_t alignment = 8) {
        size_t aligned = (offset_ + alignment - 1) & ~(alignment - 1);
        if (aligned + size > capacity_) return nullptr;
        void* ptr = buffer_ + aligned;
        offset_ = aligned + size;
        return ptr;
    }

    void reset() { offset_ = 0; }
    size_t used() const { return offset_; }
};

} // namespace exd::core
