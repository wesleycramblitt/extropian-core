#include <doctest/doctest.h>
#include <exd/core/handle.hpp>

using namespace exd::core;

// Tag types for typed handles
struct TextureTag {};
struct MeshTag {};

TEST_CASE("Handle default is invalid") {
    Handle<TextureTag> h;
    CHECK_FALSE(h.valid());
    CHECK(h == Handle<TextureTag>::null());
}

TEST_CASE("Handle valid state") {
    Handle<TextureTag> h{5, 1};
    CHECK(h.valid());
    CHECK(h.index == 5);
    CHECK(h.gen == 1);
}

TEST_CASE("Handle equality") {
    Handle<TextureTag> a{1, 0};
    Handle<TextureTag> b{1, 0};
    Handle<TextureTag> c{2, 0};
    CHECK(a == b);
    CHECK(a != c);
}

TEST_CASE("Handle different tags are distinct types") {
    Handle<TextureTag> h1{1, 0};
    Handle<MeshTag> h2{1, 0};
    // Can't compare across types at compile time — just confirm they compile
    CHECK(h1.index == h2.index);
}
