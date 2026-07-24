#include <doctest/doctest.h>
#include <exd/ecs/registry.hpp>
#include <string>

using namespace exd::ecs;

// Test component types
struct Position { float x, y, z; };
struct Velocity { float vx, vy, vz; };
struct Health { float hp; };

TEST_CASE("Registry create and destroy") {
    Registry reg;
    auto e = reg.create("player");
    CHECK(reg.valid(e));
    CHECK(reg.entity_count() == 1);

    reg.destroy(e);
    CHECK_FALSE(reg.valid(e));
    CHECK(reg.entity_count() == 0);
}

TEST_CASE("Registry entity names") {
    Registry reg;
    auto e = reg.create("enemy");
    CHECK(e.name == "enemy");
}

TEST_CASE("Registry destroy invalid is no-op") {
    Registry reg;
    Entity invalid{999, 0}; // never created
    CHECK_NOTHROW(reg.destroy(invalid));
}

TEST_CASE("Registry emplace and get") {
    Registry reg;
    auto e = reg.create("obj");

    auto& p = reg.emplace<Position>(e, 1.0f, 2.0f, 3.0f);
    CHECK(p.x == 1.0f); CHECK(p.y == 2.0f); CHECK(p.z == 3.0f);

    auto& got = reg.get<Position>(e);
    CHECK(got.x == 1.0f); CHECK(got.y == 2.0f); CHECK(got.z == 3.0f);

    // get modifies
    got.x = 10.0f;
    CHECK(reg.get<Position>(e).x == 10.0f);
}

TEST_CASE("Registry has component") {
    Registry reg;
    auto e = reg.create("obj");

    CHECK_FALSE(reg.has<Position>(e));
    reg.emplace<Position>(e, 0, 0, 0);
    CHECK(reg.has<Position>(e));
    CHECK_FALSE(reg.has<Velocity>(e));
}

TEST_CASE("Registry remove component") {
    Registry reg;
    auto e = reg.create("obj");
    reg.emplace<Position>(e, 1, 2, 3);
    CHECK(reg.has<Position>(e));

    reg.remove<Position>(e);
    CHECK_FALSE(reg.has<Position>(e));

    // Remove on entity without the component is a no-op
    CHECK_NOTHROW(reg.remove<Position>(e));
    CHECK_NOTHROW(reg.remove<Velocity>(e)); // never had it
}

TEST_CASE("Registry try_get") {
    Registry reg;
    auto e = reg.create("obj");
    reg.emplace<Health>(e, 100.0f);

    auto* h = reg.try_get<Health>(e);
    REQUIRE(h != nullptr);
    CHECK(h->hp == 100.0f);

    auto* p = reg.try_get<Position>(e);
    CHECK(p == nullptr);
}

TEST_CASE("Registry try_get const") {
    Registry reg;
    auto e = reg.create("obj");
    reg.emplace<Health>(e, 50.0f);

    const Registry& cref = reg;
    const auto* h = cref.try_get<Health>(e);
    REQUIRE(h != nullptr);
    CHECK(h->hp == 50.0f);

    const auto* p = cref.try_get<Position>(e);
    CHECK(p == nullptr);
}

TEST_CASE("Registry const get") {
    Registry reg;
    auto e = reg.create("obj");
    reg.emplace<Position>(e, 4, 5, 6);

    const Registry& cref = reg;
    const auto& p = cref.get<Position>(e);
    CHECK(p.x == 4.0f); CHECK(p.y == 5.0f); CHECK(p.z == 6.0f);
}

TEST_CASE("Registry emplace on invalid throws") {
    Registry reg;
    Entity dead{0, 0};
    CHECK_THROWS_AS(reg.emplace<Position>(dead, 0, 0, 0), std::runtime_error);
}

TEST_CASE("Registry get on invalid throws") {
    Registry reg;
    Entity dead{0, 0};
    CHECK_THROWS_AS(static_cast<void>(reg.get<Position>(dead)), std::runtime_error);
}

TEST_CASE("Registry all_entities") {
    Registry reg;
    static_cast<void>(reg.create("a"));
    static_cast<void>(reg.create("b"));
    static_cast<void>(reg.create("c"));

    auto all = reg.all_entities();
    CHECK(all.size() == 3);
    CHECK(reg.entity_count() == 3);
}

TEST_CASE("Registry all_entities excludes destroyed") {
    Registry reg;
    auto e1 = reg.create("a");
    static_cast<void>(reg.create("b"));
    static_cast<void>(reg.create("c"));
    reg.destroy(e1);

    auto all = reg.all_entities();
    CHECK(all.size() == 2);
    CHECK(reg.entity_count() == 2);
}

TEST_CASE("Registry clear") {
    Registry reg;
    static_cast<void>(reg.create("a"));
    static_cast<void>(reg.create("b"));
    reg.emplace<Position>(reg.create("c"), 0, 0, 0);

    reg.clear();
    CHECK(reg.entity_count() == 0);
    CHECK(reg.all_entities().empty());
}

TEST_CASE("Registry entity reuse (free list)") {
    Registry reg;
    auto e1 = reg.create("first");
    reg.emplace<Health>(e1, 100.0f);
    reg.destroy(e1);

    // Next entity should get the same id, bumped generation
    auto e2 = reg.create("second");
    CHECK(e2.id == e1.id);
    CHECK(e2.gen == e1.gen + 1);
    CHECK(e2.name == "second");
    CHECK_FALSE(reg.has<Health>(e2)); // old component shouldn't persist
}

TEST_CASE("Registry emplace overwrites existing") {
    Registry reg;
    auto e = reg.create("obj");

    auto& p1 = reg.emplace<Position>(e, 1, 2, 3);
    CHECK(p1.x == 1.0f);

    auto& p2 = reg.emplace<Position>(e, 10, 20, 30);
    CHECK(p2.x == 10.0f);
    // Only one Position component
    CHECK(reg.get<Position>(e).x == 10.0f);
}

TEST_CASE("Registry single-component view") {
    Registry reg;
    auto e1 = reg.create("a");
    auto e2 = reg.create("b");
    auto e3 = reg.create("c");

    reg.emplace<Position>(e1, 1, 0, 0);
    reg.emplace<Position>(e3, 3, 0, 0);
    // e2 has no Position

    int count = 0;
    for (Entity e : reg.view<Position>()) {
        (void)e;
        count++;
    }
    CHECK(count == 2);
}

TEST_CASE("Registry multi-component view") {
    Registry reg;
    auto e1 = reg.create("a");
    auto e2 = reg.create("b");
    auto e3 = reg.create("c");

    reg.emplace<Position>(e1, 0, 0, 0);
    reg.emplace<Velocity>(e1, 1, 0, 0);

    reg.emplace<Position>(e2, 0, 0, 0);
    // e2 has Position but no Velocity

    reg.emplace<Velocity>(e3, 0, 0, 0);
    // e3 has Velocity but no Position

    // Only e1 has both
    int count = 0;
    for (Entity e : reg.view<Position, Velocity>()) {
        (void)e;
        count++;
    }
    CHECK(count == 1);
}

TEST_CASE("Registry view const iteration") {
    Registry reg;
    auto e = reg.create("a");
    reg.emplace<Position>(e, 1, 2, 3);

    const Registry& cref = reg;
    int count = 0;
    for (Entity e2 : cref.view<const Position>()) {
        (void)e2;
        count++;
    }
    CHECK(count == 1);
}

TEST_CASE("Registry view.each") {
    Registry reg;
    auto e1 = reg.create("a");
    auto e2 = reg.create("b");
    reg.emplace<Position>(e1, 10, 20, 30);
    reg.emplace<Position>(e2, 40, 50, 60);

    float sum_x = 0;
    reg.view<Position>().each([&](Entity, Position& p) { sum_x += p.x; });
    CHECK(sum_x == 50.0f);
}

TEST_CASE("Registry view.each with multiple components") {
    Registry reg;
    auto e1 = reg.create("a");
    auto e2 = reg.create("b");
    reg.emplace<Position>(e1, 1, 0, 0);
    reg.emplace<Velocity>(e1, 10, 0, 0);
    reg.emplace<Position>(e2, 2, 0, 0);
    reg.emplace<Velocity>(e2, 20, 0, 0);

    float sum = 0;
    reg.view<Position, Velocity>().each([&](Entity, Position& p, Velocity& v) {
        sum += p.x + v.vx;
    });
    CHECK(sum == 33.0f); // (1+10) + (2+20)
}

TEST_CASE("Registry empty view iteration") {
    Registry reg;
    // No entities with Position
    int count = 0;
    for (Entity e : reg.view<Position>()) {
        (void)e;
        count++;
    }
    CHECK(count == 0);
}
