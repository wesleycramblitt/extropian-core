#include <doctest/doctest.h>

#include <exd/core/allocator.hpp>

using namespace exd::core;

TEST_CASE("LinearAllocator allocate within capacity") {
    LinearAllocator alloc(1024);
    void* p = alloc.allocate(64);
    CHECK(p != nullptr);
    CHECK(alloc.used() == 64);
}

TEST_CASE("LinearAllocator multiple allocations") {
    LinearAllocator alloc(256);
    void* a = alloc.allocate(50);
    void* b = alloc.allocate(30);
    CHECK(a != nullptr);
    CHECK(b != nullptr);
    CHECK(a != b);
    // After 50 bytes at offset 0, next alloc at offset 50 aligns to 8 → 56,
    // then adds 30 → 86 used
    CHECK(alloc.used() >= 80);
}

TEST_CASE("LinearAllocator overflow returns nullptr") {
    LinearAllocator alloc(100);
    void* p1 = alloc.allocate(60);
    CHECK(p1 != nullptr);
    void* p2 = alloc.allocate(60); // would exceed capacity
    CHECK(p2 == nullptr);
}

TEST_CASE("LinearAllocator alignment") {
    LinearAllocator alloc(256);
    // Allocate with 16-byte alignment
    void* p = alloc.allocate(4, 16);
    CHECK(p != nullptr);
    CHECK(reinterpret_cast<uintptr_t>(p) % 16 == 0);
}

TEST_CASE("LinearAllocator reset") {
    LinearAllocator alloc(256);
    alloc.allocate(64);
    CHECK(alloc.used() == 64);

    alloc.reset();
    CHECK(alloc.used() == 0);

    // After reset, should be able to allocate the full capacity again
    void* p = alloc.allocate(200);
    CHECK(p != nullptr);
}

TEST_CASE("LinearAllocator alignment with existing offset") {
    LinearAllocator alloc(256);
    alloc.allocate(3); // offset is 3 (align 8 → 8)
    void* p = alloc.allocate(4, 16);
    CHECK(p != nullptr);
    // offset after first alloc was at 8 (aligned from 3), then 4 bytes used
    CHECK(alloc.used() >= 8 + 4);
    CHECK(reinterpret_cast<uintptr_t>(p) % 16 == 0);
}

TEST_CASE("LinearAllocator used reports correct usage") {
    LinearAllocator alloc(1024);
    CHECK(alloc.used() == 0);
    alloc.allocate(128);
    CHECK(alloc.used() == 128);
    alloc.allocate(256);
    CHECK(alloc.used() == 384);
}
