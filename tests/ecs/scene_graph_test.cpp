#include <doctest/doctest.h>
#include <exd/ecs/scene_graph.hpp>

using namespace exd::ecs;

TEST_CASE("SceneGraph creation, parenting, and queries") {
    Registry registry;
    const auto root = registry.create("root");
    const auto child = registry.create("child");
    const auto grandchild = registry.create("grandchild");
    SceneGraph scene;
    CHECK(scene.attach(root, child));
    CHECK(scene.attach(child, grandchild));
    CHECK(scene.parent(child).value() == root);
    CHECK(scene.parent(grandchild).value() == child);
    CHECK(scene.children(root) == std::vector<Entity>{child});
    CHECK(scene.contains(child));
    CHECK(scene.contains(grandchild));
    CHECK_FALSE(scene.contains(registry.create("unrelated")));
    CHECK(scene.is_descendant(grandchild, root));
    CHECK_FALSE(scene.is_descendant(root, grandchild));
    CHECK(scene.depth(root) == 0);
    CHECK(scene.depth(child) == 1);
    CHECK(scene.depth(grandchild) == 2);
    CHECK(scene.size() == 3);
    CHECK(scene.roots() == std::vector<Entity>{root});
}

TEST_CASE("SceneGraph rejects invalid relationships (self, cycles)") {
    Registry registry;
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    const auto c = registry.create("c");
    SceneGraph scene;
    CHECK_FALSE(scene.attach(a, a));          // self-parenting
    CHECK(scene.attach(a, b));
    CHECK(scene.attach(b, c));
    CHECK_FALSE(scene.attach(c, a));          // would create a cycle
    CHECK_FALSE(scene.attach(b, a));          // b is a descendant of a
    CHECK_FALSE(scene.attach(c, b));          // already attached; not a cycle
    CHECK(scene.children(a).size() == 1);
}

TEST_CASE("SceneGraph reparenting moves subtrees") {
    Registry registry;
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    const auto c = registry.create("c");
    const auto d = registry.create("d");
    SceneGraph scene;
    scene.attach(a, b);
    scene.attach(b, c);
    scene.attach(a, d);
    // Move c (with its subtree) under d.
    CHECK(scene.attach(d, c));
    CHECK(scene.parent(c).value() == d);
    CHECK(scene.children(b).empty());
    CHECK(scene.children(d) == std::vector<Entity>{c});
    // Order after reparent + append: d appended after c... children(a) = b, d
    CHECK(scene.children(a) == std::vector<Entity>{b, d});
}

TEST_CASE("SceneGraph removal detaches and promotes children to roots") {
    Registry registry;
    const auto parent = registry.create("parent");
    const auto child = registry.create("child");
    const auto grandchild = registry.create("grandchild");
    SceneGraph scene;
    scene.attach(parent, child);
    scene.attach(child, grandchild);
    scene.remove(parent);
    CHECK_FALSE(scene.contains(parent));
    CHECK_FALSE(scene.parent(child).has_value());      // child is now a root
    CHECK(scene.parent(grandchild).value() == child);   // subtree intact
    CHECK(scene.roots() == std::vector<Entity>{child});
}

TEST_CASE("SceneGraph detach makes a node a root, children stay") {
    Registry registry;
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    const auto c = registry.create("c");
    SceneGraph scene;
    scene.attach(a, b);
    scene.attach(b, c);
    CHECK(scene.detach(b));
    CHECK_FALSE(scene.parent(b).has_value());
    CHECK(scene.parent(c).value() == b);
    CHECK(scene.children(a).empty());
    CHECK_FALSE(scene.detach(b));   // already a root
}

TEST_CASE("SceneGraph sibling ordering: attach_at, reorder, move_after") {
    Registry registry;
    const auto root = registry.create("root");
    const auto x = registry.create("x");
    const auto y = registry.create("y");
    const auto z = registry.create("z");
    SceneGraph scene;
    scene.attach(root, x);
    scene.attach(root, z);                       // [x, z]
    scene.attach_at(root, y, 1);                 // [x, y, z]
    CHECK(scene.children(root) == std::vector<Entity>{x, y, z});
    CHECK(scene.reorder(z, 0));                  // [z, x, y]
    CHECK(scene.children(root) == std::vector<Entity>{z, x, y});
    CHECK(scene.move_after(x, y));               // [z, y, x]
    CHECK(scene.children(root) == std::vector<Entity>{z, y, x});
    CHECK(scene.reorder(y, 99));                 // clamps to last -> [z, x, y]
    CHECK(scene.children(root) == std::vector<Entity>{z, x, y});
    CHECK_FALSE(scene.reorder(root, 0));         // root has no parent
}

TEST_CASE("SceneGraph traversal is depth-first pre-order with depths") {
    Registry registry;
    const auto root = registry.create("root");
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    const auto a1 = registry.create("a1");
    const auto a2 = registry.create("a2");
    SceneGraph scene;
    scene.attach(root, a);
    scene.attach(root, b);
    scene.attach(a, a1);
    scene.attach(a, a2);

    std::vector<std::pair<Entity, size_t>> visited;
    scene.traverse(root, [&](Entity e, size_t depth) {
        visited.emplace_back(e, depth);
        return true;
    });
    REQUIRE(visited.size() == 5);
    CHECK(visited[0] == std::make_pair(root, 0u));
    CHECK(visited[1] == std::make_pair(a, 1u));
    CHECK(visited[2] == std::make_pair(a1, 2u));
    CHECK(visited[3] == std::make_pair(a2, 2u));
    CHECK(visited[4] == std::make_pair(b, 1u));

    // Early abort.
    size_t calls = 0;
    scene.traverse(root, [&](Entity, size_t) { ++calls; return calls < 2; });
    CHECK(calls == 2);

    // traverse_all covers every root.
    const auto other = registry.create("other");
    scene.attach(other, registry.create("other-child"));
    size_t all = 0;
    scene.traverse_all([&](Entity, size_t) { ++all; return true; });
    CHECK(all == 7);
}

TEST_CASE("SceneGraph is generation safe and prunes destroyed entities") {
    Registry registry;
    const auto old_parent = registry.create("old");
    const auto child = registry.create("child");
    SceneGraph scene;
    scene.attach(old_parent, child);
    registry.destroy(old_parent);
    CHECK(scene.prune(registry) == 1);
    CHECK_FALSE(scene.parent(child).has_value());

    // Same id, new generation: attach works on the new handle without
    // colliding with the pruned node.
    const auto new_parent = registry.create("new");
    CHECK(new_parent.id == old_parent.id);
    CHECK(new_parent.gen != old_parent.gen);
    CHECK(scene.attach(new_parent, child));
    CHECK(scene.children(new_parent) == std::vector<Entity>{child});
}

TEST_CASE("SceneGraph clear empties the graph") {
    Registry registry;
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    SceneGraph scene;
    scene.attach(a, b);
    scene.clear();
    CHECK(scene.size() == 0);
    CHECK(scene.roots().empty());
}
