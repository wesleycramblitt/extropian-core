#include <doctest/doctest.h>
#include <exd/ecs/transform_system.hpp>
#include <exd/math/quat.hpp>

using namespace exd::ecs;
using exd::math::Quat;
using exd::math::Vec3f;

namespace {
bool quat_eq(const Quat& a, const Quat& b, float eps = 1e-4f) {
    return std::abs(a.w - b.w) < eps && std::abs(a.x - b.x) < eps &&
           std::abs(a.y - b.y) < eps && std::abs(a.z - b.z) < eps;
}
}

TEST_CASE("TransformSystem: root entity world == local") {
    Registry registry;
    SceneGraph scene;
    const auto e = registry.create("e");
    registry.emplace<LocalTransform>(e, Vec3f{1, 2, 3}, Quat{1, 0, 0, 0}, Vec3f{2, 2, 2});
    const auto world = TransformSystem::derive(registry, scene, e);
    CHECK(world.position == Vec3f{1, 2, 3});
    CHECK(quat_eq(world.rotation, Quat{1, 0, 0, 0}));
    CHECK(world.scale == Vec3f{2, 2, 2});
}

TEST_CASE("TransformSystem: nested hierarchy accumulates TRS") {
    Registry registry;
    SceneGraph scene;
    const auto parent = registry.create("parent");
    const auto child = registry.create("child");
    // Parent: translate +5 on X, scale 2. Child: local translate +1 on Y.
    registry.emplace<LocalTransform>(parent, Vec3f{5, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{2, 2, 2});
    registry.emplace<LocalTransform>(child, Vec3f{0, 1, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    scene.attach(parent, child);

    const auto world = TransformSystem::derive(registry, scene, child);
    // Parent scale 2 applies to child's local offset: (5 + 2*0, 0 + 2*1, 0)
    CHECK(world.position == Vec3f{5, 2, 0});
    CHECK(world.scale == Vec3f{2, 2, 2});
    CHECK(quat_eq(world.rotation, Quat{1, 0, 0, 0}));
}

TEST_CASE("TransformSystem: rotation composes through hierarchy") {
    Registry registry;
    SceneGraph scene;
    const auto parent = registry.create("parent");
    const auto child = registry.create("child");
    // Parent rotated 90 deg around Z. Child offset along X becomes +Y world.
    const Quat rot_z90{0.70710678f, 0, 0, 0.70710678f};
    registry.emplace<LocalTransform>(parent, Vec3f{0, 0, 0}, rot_z90, Vec3f{1, 1, 1});
    registry.emplace<LocalTransform>(child, Vec3f{1, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    scene.attach(parent, child);

    const auto world = TransformSystem::derive(registry, scene, child);
    CHECK(std::abs(world.position.x) < 1e-4f);
    CHECK(std::abs(world.position.y - 1.0f) < 1e-4f);
    CHECK(quat_eq(world.rotation, rot_z90));
}

TEST_CASE("TransformSystem: three-level chain") {
    Registry registry;
    SceneGraph scene;
    const auto g = registry.create("g");
    const auto p = registry.create("p");
    const auto c = registry.create("c");
    registry.emplace<LocalTransform>(g, Vec3f{1, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    registry.emplace<LocalTransform>(p, Vec3f{2, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    registry.emplace<LocalTransform>(c, Vec3f{4, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    scene.attach(g, p);
    scene.attach(p, c);
    const auto world = TransformSystem::derive(registry, scene, c);
    CHECK(world.position == Vec3f{7, 0, 0});
}

TEST_CASE("TransformSystem: reparenting changes derived transform") {
    Registry registry;
    SceneGraph scene;
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    const auto c = registry.create("c");
    registry.emplace<LocalTransform>(a, Vec3f{10, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    registry.emplace<LocalTransform>(b, Vec3f{0, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    registry.emplace<LocalTransform>(c, Vec3f{1, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    scene.attach(a, c);
    CHECK(TransformSystem::derive(registry, scene, c).position == Vec3f{11, 0, 0});
    scene.attach(b, c);  // reparent
    CHECK(TransformSystem::derive(registry, scene, c).position == Vec3f{1, 0, 0});
}

TEST_CASE("TransformSystem: missing LocalTransform leaves entity untouched") {
    Registry registry;
    SceneGraph scene;
    const auto e = registry.create("e");
    const auto w = registry.create("w");
    registry.emplace<WorldTransform>(w, Vec3f{9, 9, 9}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    TransformSystem sys(scene);
    sys.update(registry, 0.0);
    // e has no LocalTransform -> no WorldTransform written.
    CHECK_FALSE(registry.has<WorldTransform>(e));
    // w has WorldTransform but no LocalTransform -> left as authored.
    CHECK(registry.get<WorldTransform>(w).position == Vec3f{9, 9, 9});
    // e gains a LocalTransform on the next update -> WorldTransform appears.
    registry.emplace<LocalTransform>(e, Vec3f{3, 3, 3}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    sys.update(registry, 0.0);
    CHECK(registry.has<WorldTransform>(e));
    CHECK(registry.get<WorldTransform>(e).position == Vec3f{3, 3, 3});
}

TEST_CASE("TransformSystem: stale parent treated as root") {
    Registry registry;
    SceneGraph scene;
    const auto parent = registry.create("parent");
    const auto child = registry.create("child");
    registry.emplace<LocalTransform>(parent, Vec3f{5, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    registry.emplace<LocalTransform>(child, Vec3f{1, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    scene.attach(parent, child);
    CHECK(TransformSystem::derive(registry, scene, child).position == Vec3f{6, 0, 0});
    registry.destroy(parent);
    scene.prune(registry);
    CHECK(TransformSystem::derive(registry, scene, child).position == Vec3f{1, 0, 0});
}

TEST_CASE("TransformSystem: cycle guard") {
    Registry registry;
    SceneGraph scene;
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    registry.emplace<LocalTransform>(a, Vec3f{1, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    registry.emplace<LocalTransform>(b, Vec3f{2, 0, 0}, Quat{1, 0, 0, 0}, Vec3f{1, 1, 1});
    scene.attach(a, b);
    // Force a malformed cycle by bypassing attach() validation.
    scene.attach(b, a);  // rejected... verify chain stays valid
    CHECK(scene.parent(a) == std::nullopt);
    const auto world = TransformSystem::derive(registry, scene, a);
    CHECK(world.position == Vec3f{1, 0, 0});  // terminates
}
