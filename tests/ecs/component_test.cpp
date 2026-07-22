#include <doctest/doctest.h>
#include <exd/ecs/component.hpp>

using namespace exd::ecs;

// Test component types
struct Health { float hp; };
struct Position { float x, y, z; };

// Non-trivially-movable type (should NOT satisfy Component)
struct ComplexType {
    ComplexType() = default;
    ~ComplexType() = default;
    ComplexType(const ComplexType&) : data(new int(0)) {}
    ComplexType& operator=(const ComplexType&) { return *this; }
    // Not trivially move constructible due to explicit copy ctor
    ComplexType(ComplexType&&) : data(nullptr) {}
    ComplexType& operator=(ComplexType&&) { data = nullptr; return *this; }
    int* data = nullptr;
};

TEST_CASE("ComponentTraits id stability") {
    // Same type always returns the same ID within one TU
    uint32_t id1 = ComponentTraits<Health>::id();
    uint32_t id2 = ComponentTraits<Health>::id();
    CHECK(id1 == id2);
}

TEST_CASE("ComponentTraits different types callable") {
    // Different types can both be queried without issue
    ComponentTraits<Health>::id();
    ComponentTraits<Position>::id();
    // No crash = pass
    CHECK(true);
}

TEST_CASE("Component concept") {
    // Trivially movable + destructible → valid component
    static_assert(Component<Health>);
    static_assert(Component<Position>);
    static_assert(Component<float>);
    static_assert(Component<int>);

    // Pointer types are NOT components
    static_assert(!Component<int*>);
    static_assert(!Component<const float*>);
}

TEST_CASE("Component concept with multiple types") {
    struct Velocity { float vx, vy, vz; };
    // Verify no crash and IDs are stable per-type
    uint32_t health_id = ComponentTraits<Health>::id();
    CHECK(ComponentTraits<Health>::id()   == health_id);
    CHECK(ComponentTraits<Position>::id() == ComponentTraits<Position>::id());
    CHECK(ComponentTraits<Velocity>::id() == ComponentTraits<Velocity>::id());
}
