#include <doctest/doctest.h>
#include <exd/ecs/hierarchy_index.hpp>

using namespace exd::ecs;

TEST_CASE("HierarchyIndex stores parents and children") {
    Registry registry;
    const auto root = registry.create("root");
    const auto child = registry.create("child");
    const auto grandchild = registry.create("grandchild");
    HierarchyIndex index;
    CHECK(index.set_parent(child, root));
    CHECK(index.set_parent(grandchild, child));
    CHECK(index.parent(child).value() == root);
    CHECK(index.children(root) == std::vector<Entity>{child});
    CHECK(index.is_descendant(grandchild, root));
}

TEST_CASE("HierarchyIndex rejects cycles and supports reparenting") {
    Registry registry;
    const auto a = registry.create("a");
    const auto b = registry.create("b");
    const auto c = registry.create("c");
    HierarchyIndex index;
    CHECK(index.set_parent(b, a));
    CHECK(index.set_parent(c, a));
    CHECK(index.set_parent(c, b));
    CHECK_FALSE(index.set_parent(a, c));
    CHECK(index.children(a).size() == 1);
    CHECK(index.children(a)[0] == b);
}

TEST_CASE("HierarchyIndex is generation safe and prunes stale entities") {
    Registry registry;
    const auto old_parent = registry.create("old");
    const auto child = registry.create("child");
    HierarchyIndex index;
    CHECK(index.set_parent(child, old_parent));
    registry.destroy(old_parent);
    CHECK(index.prune(registry) == 1);
    CHECK_FALSE(index.parent(child).has_value());

    const auto new_parent = registry.create("new");
    CHECK(new_parent.id == old_parent.id);
    CHECK(new_parent.gen != old_parent.gen);
    CHECK(index.set_parent(child, new_parent));
    CHECK(index.children(new_parent)[0] == child);
}

TEST_CASE("HierarchyIndex removal promotes children to roots") {
    Registry registry;
    const auto parent = registry.create("parent");
    const auto child = registry.create("child");
    HierarchyIndex index;
    CHECK(index.set_parent(child, parent));
    index.remove(parent);
    CHECK_FALSE(index.contains(parent));
    CHECK_FALSE(index.parent(child).has_value());
}
