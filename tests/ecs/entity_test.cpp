#include <doctest/doctest.h>
#include <exd/ecs/registry.hpp>

using namespace exd::ecs;

TEST_CASE("Entity default state") {
    Entity e;
    CHECK(e.id == Entity::id_type(std::numeric_limits<Entity::id_type>::max()));
    CHECK(e.gen == 0);
    CHECK(e.name.empty());
}

TEST_CASE("Entity equality") {
    Entity a{1, 0, "foo"};
    Entity b{1, 0, "bar"}; // same id+gen, different name
    Entity c{2, 0, "foo"};

    CHECK(a == b);
    CHECK_FALSE(a == c);
    CHECK_FALSE(a != b);
    CHECK(a != c);
}

TEST_CASE("Entity name is independent of equality") {
    Entity a{1, 0, "alpha"};
    Entity b{1, 0, "beta"};
    // Names differ but equality is based on id+gen only
    CHECK(a == b);
}
