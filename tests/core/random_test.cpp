#include <doctest/doctest.h>
#include <exd/core/random.hpp>

using namespace exd::core;

TEST_CASE("RNG deterministic with seed") {
    RNG a(42);
    RNG b(42);
    CHECK(a.next_u32() == b.next_u32());
    CHECK(a.next_u32() == b.next_u32());
}

TEST_CASE("RNG different seeds diverge") {
    RNG a(1);
    RNG b(2);
    // Very unlikely to produce same sequence
    bool same = true;
    for (int i = 0; i < 10; ++i) {
        if (a.next_u32() != b.next_u32()) { same = false; break; }
    }
    CHECK_FALSE(same);
}

TEST_CASE("RNG next_f32 in range") {
    RNG rng(123);
    for (int i = 0; i < 100; ++i) {
        float v = rng.next_f32();
        CHECK(v >= 0.0f);
        CHECK(v < 1.0f);
    }
}

TEST_CASE("RNG next_f32_range") {
    RNG rng(456);
    for (int i = 0; i < 100; ++i) {
        float v = rng.next_f32_range(5.0f, 10.0f);
        CHECK(v >= 5.0f);
        CHECK(v < 10.0f);
    }
}

TEST_CASE("RNG next_int") {
    RNG rng(789);
    for (int i = 0; i < 100; ++i) {
        int v = rng.next_int(1, 6); // dice roll
        CHECK(v >= 1);
        CHECK(v <= 6);
    }
}

TEST_CASE("RNG chance") {
    RNG rng(42);
    int yes = 0, total = 1000;
    for (int i = 0; i < total; ++i) {
        if (rng.chance(0.3f)) yes++;
    }
    // Should be roughly 300, allow generous margin
    CHECK(yes > 150);
    CHECK(yes < 450);
}

TEST_CASE("RNG fill") {
    RNG rng;
    int val = 0;
    rng.fill(val);
    // Just ensure it doesn't crash — val is overwritten with random bytes
    (void)val;
}

TEST_CASE("Random uniform") {
    Random rnd(42);
    for (int i = 0; i < 100; ++i) {
        float v = rnd.uniform();
        CHECK(v >= 0.0f); CHECK(v <= 1.0f);
    }
}

TEST_CASE("Random uniform_int") {
    Random rnd(42);
    for (int i = 0; i < 100; ++i) {
        int v = rnd.uniform_int(0, 10);
        CHECK(v >= 0); CHECK(v <= 10);
    }
}

TEST_CASE("Random normal") {
    Random rnd(42);
    for (int i = 0; i < 100; ++i) {
        float v = rnd.normal(5.0f, 1.0f);
        // Should be finite and not too crazy
        CHECK(std::isfinite(v));
    }
}

TEST_CASE("Random chance") {
    Random rnd(42);
    int hits = 0;
    for (int i = 0; i < 1000; ++i) {
        if (rnd.chance(0.5f)) hits++;
    }
    CHECK(hits > 350);
    CHECK(hits < 650);
}
