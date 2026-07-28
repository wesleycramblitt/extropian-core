#include <exd/core/allocator.hpp>
#include <cstdlib>

namespace exd::core {

LinearAllocator::LinearAllocator(size_t capacity) : capacity_(capacity) {
    buffer_ = static_cast<uint8_t*>(std::malloc(capacity));
}

LinearAllocator::~LinearAllocator() { std::free(buffer_); }

void* LinearAllocator::allocate(size_t size, size_t alignment) {
    size_t aligned = (offset_ + alignment - 1) & ~(alignment - 1);
    if (aligned + size > capacity_) return nullptr;
    void* ptr = buffer_ + aligned;
    offset_ = aligned + size;
    return ptr;
}

void LinearAllocator::reset() { offset_ = 0; }

size_t LinearAllocator::used() const { return offset_; }

} // namespace exd::core
