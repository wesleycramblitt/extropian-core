#include <doctest/doctest.h>
#include <exd/ecs/system_graph.hpp>

using namespace exd::ecs;

struct TestSystem : ISystem {
    int call_count = 0;
    double last_dt = 0.0;

    void update(Registry&, double dt) override {
        call_count++;
        last_dt = dt;
    }
};

TEST_CASE("SystemGraph add and update") {
    SystemGraph graph;
    auto& sys = graph.add<TestSystem>();
    CHECK(graph.count() == 1);

    Registry reg;
    graph.update(reg, 0.016);
    CHECK(sys.call_count == 1);
    CHECK(sys.last_dt == doctest::Approx(0.016));
}

TEST_CASE("SystemGraph multiple systems in order") {
    SystemGraph graph;
    std::vector<int> order;

    struct Ordered : ISystem {
        std::vector<int>* order;
        int id;
        Ordered(std::vector<int>* o, int i) : order(o), id(i) {}
        void update(Registry&, double) override { order->push_back(id); }
    };

    graph.add<Ordered>(&order, 1);
    graph.add<Ordered>(&order, 2);
    graph.add<Ordered>(&order, 3);

    Registry reg;
    graph.update(reg, 0.0);
    REQUIRE(order.size() == 3);
    CHECK(order[0] == 1);
    CHECK(order[1] == 2);
    CHECK(order[2] == 3);
}

TEST_CASE("SystemGraph add_ref") {
    SystemGraph graph;
    TestSystem sys;
    graph.add_ref(&sys);

    Registry reg;
    graph.update(reg, 0.0);
    CHECK(sys.call_count == 1);
}

TEST_CASE("SystemGraph clear") {
    SystemGraph graph;
    graph.add<TestSystem>();
    CHECK(graph.count() == 1);
    graph.clear();
    CHECK(graph.count() == 0);
}
