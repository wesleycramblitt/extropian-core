#include <doctest/doctest.h>
#include <exd/ecs/command_buffer.hpp>

using namespace exd::ecs;

struct Health { float hp; };

TEST_CASE("CommandBuffer create entity") {
    Registry reg;
    CommandBuffer cmd(reg);
    CHECK(cmd.empty());

    cmd.create("player");
    CHECK(cmd.pending() == 1);

    // Entity not in registry yet
    CHECK(reg.entity_count() == 0);

    cmd.execute();
    CHECK(reg.entity_count() == 1);
    CHECK(cmd.empty());
}

TEST_CASE("CommandBuffer destroy entity") {
    Registry reg;
    auto e = reg.create("enemy");
    CommandBuffer cmd(reg);

    cmd.destroy(e);
    CHECK(reg.valid(e)); // still alive before execute

    cmd.execute();
    CHECK_FALSE(reg.valid(e)); // dead after execute
}

TEST_CASE("CommandBuffer emplace component") {
    Registry reg;
    auto e = reg.create("obj");
    CommandBuffer cmd(reg);

    cmd.emplace<Health>(e, 100.0f);
    CHECK_FALSE(reg.has<Health>(e));

    cmd.execute();
    CHECK(reg.has<Health>(e));
    CHECK(reg.get<Health>(e).hp == 100.0f);
}

TEST_CASE("CommandBuffer remove component") {
    Registry reg;
    auto e = reg.create("obj");
    reg.emplace<Health>(e, 50.0f);
    CommandBuffer cmd(reg);

    cmd.remove<Health>(e);
    CHECK(reg.has<Health>(e));

    cmd.execute();
    CHECK_FALSE(reg.has<Health>(e));
}

TEST_CASE("CommandBuffer multiple commands") {
    Registry reg;
    CommandBuffer cmd(reg);

    cmd.create("a");
    cmd.create("b");
    cmd.create("c");

    CHECK(cmd.pending() == 3);
    cmd.execute();
    CHECK(reg.entity_count() == 3);
}

TEST_CASE("CommandBuffer clear discards commands") {
    Registry reg;
    CommandBuffer cmd(reg);
    cmd.create("doomed");
    cmd.clear();
    CHECK(cmd.empty());
    cmd.execute();
    CHECK(reg.entity_count() == 0);
}
