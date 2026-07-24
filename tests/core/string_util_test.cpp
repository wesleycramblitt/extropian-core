#include <doctest/doctest.h>
#include <exd/core/string_util.hpp>

using namespace exd::core::str;

TEST_CASE("to_lower") {
    CHECK(to_lower("Hello") == "hello");
    CHECK(to_lower("HELLO") == "hello");
    CHECK(to_lower("") == "");
}

TEST_CASE("to_upper") {
    CHECK(to_upper("hello") == "HELLO");
    CHECK(to_upper("Hello") == "HELLO");
}

TEST_CASE("trim") {
    CHECK(trim("  hello  ") == "hello");
    CHECK(trim("no_spaces") == "no_spaces");
    CHECK(trim("   ") == "");
    CHECK(trim("") == "");
}

TEST_CASE("trim_left") {
    CHECK(trim_left("  hello") == "hello");
    CHECK(trim_left("hello  ") == "hello  ");
}

TEST_CASE("trim_right") {
    CHECK(trim_right("hello  ") == "hello");
    CHECK(trim_right("  hello") == "  hello");
}

TEST_CASE("starts_with") {
    CHECK(starts_with("hello world", "hello"));
    CHECK_FALSE(starts_with("hello world", "world"));
    CHECK(starts_with("hello", ""));
    CHECK_FALSE(starts_with("", "hello"));
}

TEST_CASE("ends_with") {
    CHECK(ends_with("hello world", "world"));
    CHECK_FALSE(ends_with("hello world", "hello"));
    CHECK(ends_with("hello", ""));
}

TEST_CASE("split") {
    auto parts = split("a,b,c", ',');
    REQUIRE(parts.size() == 3);
    CHECK(parts[0] == "a");
    CHECK(parts[1] == "b");
    CHECK(parts[2] == "c");

    auto single = split("hello", ',');
    REQUIRE(single.size() == 1);
    CHECK(single[0] == "hello");

    auto empty = split("", ',');
    REQUIRE(empty.size() == 1);
    CHECK(empty[0] == "");
}

TEST_CASE("split_whitespace") {
    auto parts = split_whitespace("hello  world\tfoo");
    REQUIRE(parts.size() == 3);
    CHECK(parts[0] == "hello");
    CHECK(parts[1] == "world");
    CHECK(parts[2] == "foo");
}

TEST_CASE("join") {
    std::vector<std::string> parts = {"a", "b", "c"};
    CHECK(join(parts, ",") == "a,b,c");
    CHECK(join(parts, " | ") == "a | b | c");
}

TEST_CASE("replace_all") {
    CHECK(replace_all("hello world world", "world", "earth") == "hello earth earth");
    CHECK(replace_all("aaa", "a", "bb") == "bbbbbb");
    CHECK(replace_all("no match", "x", "y") == "no match");
}

TEST_CASE("contains") {
    CHECK(contains("hello world", "world"));
    CHECK_FALSE(contains("hello world", "earth"));
    CHECK(contains("hello", ""));
}
