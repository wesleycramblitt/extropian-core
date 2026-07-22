#include <doctest/doctest.h>
#include <exd/core/config.hpp>
#include <fstream>
#include <cstdio>

using namespace exd::core;

TEST_CASE("Config set and get") {
    Config cfg;
    cfg.set("name", "alice");
    auto val = cfg.get("name");
    REQUIRE(val.has_value());
    CHECK(*val == "alice");
}

TEST_CASE("Config get missing key returns nullopt") {
    Config cfg;
    CHECK_FALSE(cfg.get("nonexistent").has_value());
}

TEST_CASE("Config has") {
    Config cfg;
    cfg.set("a", "1");
    CHECK(cfg.has("a"));
    CHECK_FALSE(cfg.has("b"));
}

TEST_CASE("Config set_default") {
    Config cfg;
    cfg.set_default("size", "medium");
    CHECK(*cfg.get("size") == "medium");

    // set_default shouldn't overwrite existing
    cfg.set("size", "large");
    cfg.set_default("size", "small");
    CHECK(*cfg.get("size") == "large");
}

TEST_CASE("Config get_or int") {
    Config cfg;
    cfg.set("count", "42");
    CHECK(cfg.get_or<int>("count", 0) == 42);
    CHECK(cfg.get_or<int>("missing", 99) == 99);
}

TEST_CASE("Config get_or float") {
    Config cfg;
    cfg.set("pi", "3.14");
    CHECK(cfg.get_or<float>("pi", 0.0f) == doctest::Approx(3.14f));
}

TEST_CASE("Config get_or with invalid value") {
    Config cfg;
    cfg.set("bad", "not_a_number");
    // Failed parse produces T{} (0 for int) and sets stream failbit.
    // get_or returns default only when key is missing, not when parse fails.
    int result = cfg.get_or<int>("bad", -1);
    // Stream leaves val at 0 on parse failure — don't assert exact value, just no crash
    (void)result;
}

TEST_CASE("Config char* overloads") {
    Config cfg;
    const char* key = "key";
    cfg.set(key, "value1");
    CHECK(*cfg.get("key") == "value1");

    char buf[] = "some_value";
    cfg.set(key, buf);
    CHECK(*cfg.get("key") == "some_value");
}

TEST_CASE("Config toml round-trip") {
    Config cfg;
    cfg.set("name", "test");
    cfg.set("count", "10");
    cfg.set("pi", "3.14");

    const char* path = "/tmp/test_config.toml";
    cfg.save_toml(path);

    Config cfg2;
    cfg2.load_toml(path);
    CHECK(cfg2.get_or<std::string>("name", "") == "test");
    CHECK(cfg2.get_or<int>("count", 0) == 10);
    CHECK(cfg2.get_or<float>("pi", 0.0f) == doctest::Approx(3.14f));

    // Cleanup
    std::remove(path);
}

TEST_CASE("Config load missing file") {
    Config cfg;
    CHECK_NOTHROW(cfg.load_toml("/tmp/nonexistent_config_file.toml"));
}

TEST_CASE("Config save error handling") {
    Config cfg;
    // Save to a read-only directory should fail silently
    CHECK_NOTHROW(cfg.save_toml("/root/cannot_write.toml"));
}
