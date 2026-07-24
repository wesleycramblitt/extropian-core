#include <doctest/doctest.h>
#include <exd/core/pool.hpp>

using namespace exd::core;

struct Node { int x, y; };

TEST_CASE("Pool allocate and deallocate") {
    Pool<Node> pool;
    Node* n = pool.allocate();
    REQUIRE(n != nullptr);
    n->x = 42;
    n->y = 99;

    CHECK(n->x == 42);
    CHECK(n->y == 99);

    pool.deallocate(n);
    // After deallocation, pointer should be reusable
    Node* n2 = pool.allocate();
    REQUIRE(n2 != nullptr);
    // Might get the same slot back
    n2->x = 100;
    CHECK(n2->x == 100);
}

TEST_CASE("Pool allocate many") {
    Pool<int> pool;
    std::vector<int*> ptrs;
    for (int i = 0; i < 1000; ++i) {
        int* p = pool.allocate();
        REQUIRE(p != nullptr);
        *p = i;
        ptrs.push_back(p);
    }
    for (size_t i = 0; i < ptrs.size(); ++i) {
        CHECK(*ptrs[i] == static_cast<int>(i));
    }
}

TEST_CASE("Pool deallocate and reuse") {
    Pool<int> pool;
    // Allocate, deallocate, allocate — should reuse
    int* a = pool.allocate();
    *a = 1;
    pool.deallocate(a);

    int* b = pool.allocate();
    // b might or might not be the same as a, but both should work
    *b = 2;
    CHECK(*b == 2);
}

TEST_CASE("Pool clear") {
    Pool<int> pool;
    int* p = pool.allocate();
    pool.clear();
    // After clear, new alloc should work (get a fresh chunk)
    int* p2 = pool.allocate();
    REQUIRE(p2 != nullptr);
    *p2 = 99;
    CHECK(*p2 == 99);
}

TEST_CASE("Pool nullptr deallocate safe") {
    Pool<int> pool;
    pool.deallocate(nullptr); // should not crash
}
