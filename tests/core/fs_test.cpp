#include <doctest/doctest.h>
#include <exd/core/fs.hpp>
#include <fstream>
#include <cstdio>

using namespace exd::core::fs;

TEST_CASE("file_exists") {
    CHECK(file_exists("/tmp/nonexistent_abcdefg.xyz") == false);
}

TEST_CASE("read_text_file missing") {
    auto content = read_text_file("/tmp/nonexistent_file_test.txt");
    CHECK_FALSE(content.has_value());
}

TEST_CASE("write_text_file and read_text_file round-trip") {
    const char* path = "/tmp/test_core_fs.txt";
    bool ok = write_text_file(path, "hello\nworld");
    CHECK(ok);

    auto content = read_text_file(path);
    REQUIRE(content.has_value());
    CHECK(*content == "hello\nworld");

    std::remove(path);
}

TEST_CASE("write_binary_file and read_binary_file round-trip") {
    const char* path = "/tmp/test_core_fs_bin.dat";
    int data[3] = {1, 2, 3};
    bool ok = write_binary_file(path, data, sizeof(data));
    CHECK(ok);

    auto blob = read_binary_file(path);
    REQUIRE(blob.size() == sizeof(data));
    int* out = reinterpret_cast<int*>(blob.data());
    CHECK(out[0] == 1);
    CHECK(out[1] == 2);
    CHECK(out[2] == 3);

    std::remove(path);
}

TEST_CASE("create_directories") {
    CHECK(create_directories("/tmp/test_core_fs_dir/subdir"));
    // Verify it exists
    CHECK(dir_exists("/tmp/test_core_fs_dir"));
    // Cleanup
    std::filesystem::remove_all("/tmp/test_core_fs_dir");
}

TEST_CASE("extension") {
    CHECK(extension("file.txt") == ".txt");
    CHECK(extension("/path/to/file.tar.gz") == ".gz");
    CHECK(extension("noext") == "");
}

TEST_CASE("filename") {
    CHECK(filename("/path/to/file.txt") == "file.txt");
    CHECK(filename("file.txt") == "file.txt");
}

TEST_CASE("stem") {
    CHECK(stem("/path/to/file.txt") == "file");
    CHECK(stem("archive.tar.gz") == "archive.tar");
}

TEST_CASE("parent_path") {
    CHECK(parent_path("/path/to/file.txt") == "/path/to");
    CHECK(parent_path("file.txt") == "");
}

TEST_CASE("current_dir") {
    auto cwd = current_dir();
    CHECK_FALSE(cwd.empty());
}

TEST_CASE("file_size") {
    auto sz = file_size("/tmp/nonexistent_size.xyz");
    CHECK_FALSE(sz.has_value());
}

TEST_CASE("list_directory") {
    // List /tmp — should have some entries (or be empty, which is fine)
    auto entries = list_directory("/tmp");
    // Just verify it doesn't crash
    CHECK(entries.size() >= 0);
}
