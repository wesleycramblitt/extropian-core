#include <doctest/doctest.h>
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <fstream>

#include "core/serialization.h"

using namespace exd::core;

TEST_CASE("serialize_binary round-trip") {
    int input = 42;
    auto blob = serialize_binary(&input, sizeof(int));
    REQUIRE(blob.size() == sizeof(int));

    int output = 0;
    deserialize_binary(blob, &output, sizeof(int));
    CHECK(output == 42);
}

TEST_CASE("serialize_binary zero size") {
    auto blob = serialize_binary(nullptr, 0);
    CHECK(blob.empty());
}

TEST_CASE("serialize_binary struct") {
    struct Data { float a; int b; };
    Data in{3.14f, 99};
    auto blob = serialize_binary(&in, sizeof(Data));

    Data out{};
    deserialize_binary(blob, &out, sizeof(Data));
    CHECK(out.a == doctest::Approx(3.14f));
    CHECK(out.b == 99);
}

TEST_CASE("deserialize_binary smaller blob") {
    uint8_t small_blob[2] = {0x41, 0x42};
    std::vector<uint8_t> vec(small_blob, small_blob + 2);

    int out = -1;
    // sizeof(int) is 4, blob is only 2 — should not overwrite
    deserialize_binary(vec, &out, sizeof(int));
    CHECK(out == -1); // unchanged since blob is too small
}

TEST_CASE("write_binary_file and read_binary_file round-trip") {
    const char* path = "/tmp/test_binary.dat";
    int value = 12345;
    auto blob = serialize_binary(&value, sizeof(int));

    write_binary_file(path, blob);

    auto read_back = read_binary_file(path);
    REQUIRE(read_back.size() == sizeof(int));

    int out = 0;
    std::memcpy(&out, read_back.data(), sizeof(int));
    CHECK(out == 12345);

    std::remove(path);
}

TEST_CASE("read_binary_file missing file") {
    auto result = read_binary_file("/tmp/nonexistent_binary_test_file.dat");
    CHECK(result.empty());
}

TEST_CASE("json_escape plain text") {
    CHECK(json_escape("hello") == "hello");
    CHECK(json_escape("") == "");
}

TEST_CASE("json_escape quotes") {
    CHECK(json_escape("say \"hi\"") == "say \\\"hi\\\"");
}

TEST_CASE("json_escape backslashes") {
    std::string result = json_escape("a\\b");
    CHECK(result == "a\\\\b");
}

TEST_CASE("json_escape mixed") {
    std::string result = json_escape("key\"value\"\\end");
    CHECK(result == "key\\\"value\\\"\\\\end");
}
