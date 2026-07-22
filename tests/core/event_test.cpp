#include <doctest/doctest.h>

#include "core/event.h"

using namespace exd::core;

TEST_CASE("EventBus on and emit") {
    EventBus bus;
    int call_count = 0;
    bus.on("test", [&](const void*) { call_count++; });

    bus.emit("test");
    CHECK(call_count == 1);

    bus.emit("test");
    CHECK(call_count == 2);
}

TEST_CASE("EventBus multiple listeners") {
    EventBus bus;
    int a = 0, b = 0;
    bus.on("event", [&](const void*) { a++; });
    bus.on("event", [&](const void*) { b++; });

    bus.emit("event");
    CHECK(a == 1);
    CHECK(b == 1);
}

TEST_CASE("EventBus emit with no listeners") {
    EventBus bus;
    CHECK_NOTHROW(bus.emit("no_listeners"));
}

TEST_CASE("EventBus unregistered event no-op") {
    EventBus bus;
    int fired = 0;
    bus.on("alpha", [&](const void*) { fired++; });

    bus.emit("beta"); // different event
    CHECK(fired == 0);
}

TEST_CASE("EventBus payload passing") {
    EventBus bus;
    int received = 0;
    bus.on("data", [&](const void* payload) {
        if (payload) received = *static_cast<const int*>(payload);
    });

    int value = 42;
    bus.emit("data", &value);
    CHECK(received == 42);
}

TEST_CASE("EventBus clear") {
    EventBus bus;
    int count = 0;
    bus.on("test", [&](const void*) { count++; });

    bus.emit("test");
    CHECK(count == 1);

    bus.clear();
    bus.emit("test");
    CHECK(count == 1); // no change after clear
}
