#include <doctest/doctest.h>
#include <exd/core/hash.hpp>
#include <string>

using namespace exd::core;

TEST_CASE("fnv1a_64_str known values") {
    // Empty string FNV-1a 64 hash
    CHECK(fnv1a_64_str("") == 0xcbf29ce484222325ULL);
}

TEST_CASE("fnv1a_64_str deterministic") {
    CHECK(fnv1a_64_str("hello") == fnv1a_64_str("hello"));
    CHECK(fnv1a_64_str("hello") != fnv1a_64_str("world"));
}

TEST_CASE("fnv1a_32_str deterministic") {
    CHECK(fnv1a_32_str("test") == fnv1a_32_str("test"));
    CHECK(fnv1a_32_str("a") != fnv1a_32_str("b"));
}

TEST_CASE("hash_combine") {
    size_t h1 = 0;
    hash_combine(h1, 42);
    hash_combine(h1, std::string("hello"));

    size_t h2 = 0;
    hash_combine(h2, 42);
    hash_combine(h2, std::string("hello"));

    CHECK(h1 == h2);

    size_t h3 = 0;
    hash_combine(h3, 99);
    hash_combine(h3, std::string("hello"));
    CHECK(h1 != h3);
}

TEST_CASE("hash_all") {
    size_t a = hash_all(1, 2, 3);
    size_t b = hash_all(1, 2, 3);
    CHECK(a == b);
    size_t c = hash_all(3, 2, 1);
    CHECK(a != c); // order matters
}

TEST_CASE("hash_bytes") {
    int val = 42;
    uint64_t h = hash_bytes(val);
    CHECK(h != 0); // should produce a non-zero hash
}
