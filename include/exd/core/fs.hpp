#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <optional>
#include <filesystem>

namespace exd::core::fs {

// ────────────────────────────────────────────────────
//  File-system utilities (thin wrappers over std::filesystem)
// ────────────────────────────────────────────────────

/// Check whether a regular file exists at `path`.
[[nodiscard]] inline bool file_exists(std::string_view path) {
    std::error_code ec;
    return std::filesystem::is_regular_file(path, ec);
}

/// Check whether a directory exists at `path`.
[[nodiscard]] inline bool dir_exists(std::string_view path) {
    std::error_code ec;
    return std::filesystem::is_directory(path, ec);
}

/// Read an entire text file into a string. Returns nullopt on failure.
[[nodiscard]] std::optional<std::string> read_text_file(std::string_view path);

/// Read an entire binary file into a byte vector. Returns empty vector on failure.
[[nodiscard]] std::vector<uint8_t> read_binary_file(std::string_view path);

/// Write a string to a file (overwrites). Returns true on success.
[[nodiscard]] bool write_text_file(std::string_view path, std::string_view content);

/// Write binary data to a file (overwrites). Returns true on success.
[[nodiscard]] bool write_binary_file(std::string_view path,
                                      const void* data, size_t size);

/// List entries in a directory (files and subdirectories).
[[nodiscard]] std::vector<std::string> list_directory(std::string_view path);

/// Create a directory (and all parents if needed). Returns true on success.
[[nodiscard]] inline bool create_directories(std::string_view path) {
    std::error_code ec;
    return std::filesystem::create_directories(path, ec);
}

/// Get the file extension (e.g. ".png", ".toml" — includes the dot).
[[nodiscard]] inline std::string extension(std::string_view path) {
    return std::filesystem::path(path).extension().string();
}

/// Get the file name (with extension).
[[nodiscard]] inline std::string filename(std::string_view path) {
    return std::filesystem::path(path).filename().string();
}

/// Get the file name without extension.
[[nodiscard]] inline std::string stem(std::string_view path) {
    return std::filesystem::path(path).stem().string();
}

/// Get the parent directory path.
[[nodiscard]] inline std::string parent_path(std::string_view path) {
    return std::filesystem::path(path).parent_path().string();
}

/// Current working directory.
[[nodiscard]] inline std::string current_dir() {
    std::error_code ec;
    return std::filesystem::current_path(ec).string();
}

/// Get file size in bytes, or nullopt if file doesn't exist.
[[nodiscard]] inline std::optional<size_t> file_size(std::string_view path) {
    std::error_code ec;
    auto sz = std::filesystem::file_size(path, ec);
    if (ec) return std::nullopt;
    return sz;
}

} // namespace exd::core::fs
