#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace exd::core {

// ────────────────────────────────────────────────────
//  Serialization — binary blob & JSON helpers
// ────────────────────────────────────────────────────

/// Serialize a block of memory into a byte vector.
std::vector<uint8_t> serialize_binary(const void* data, size_t size);

/// Deserialize a byte vector back into memory.
/// No-op if the blob is smaller than `size`.
void deserialize_binary(const std::vector<uint8_t>& blob, void* out, size_t size);

/// Write binary data to a file.
void write_binary_file(const std::string& path, const std::vector<uint8_t>& data);

/// Read a file into a byte vector. Returns empty on failure.
std::vector<uint8_t> read_binary_file(const std::string& path);

/// Escape a string for embedding in JSON (quotes and backslashes).
std::string json_escape(const std::string& s);

} // namespace exd::core
