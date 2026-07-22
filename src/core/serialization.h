#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace exd::core {

std::vector<uint8_t> serialize_binary(const void* data, size_t size);
void deserialize_binary(const std::vector<uint8_t>& blob, void* out, size_t size);
void write_binary_file(const std::string& path, const std::vector<uint8_t>& data);
std::vector<uint8_t> read_binary_file(const std::string& path);
std::string json_escape(const std::string& s);

} // namespace exd::core
