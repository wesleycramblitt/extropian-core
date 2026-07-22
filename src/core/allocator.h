#pragma once
#include <cstdint>
#include <cstddef>

namespace exd::core {

class LinearAllocator {
public:
    LinearAllocator(size_t capacity);
    ~LinearAllocator();
    void* allocate(size_t size, size_t alignment = 8);
    void reset();
    size_t used() const;

private:
    uint8_t* buffer_ = nullptr;
    size_t capacity_ = 0;
    size_t offset_ = 0;
};

} // namespace exd::core
