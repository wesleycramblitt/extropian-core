#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdint>

namespace exd::core {

// ── Simple binary blob read/write ──

std::vector<uint8_t> serialize_binary(const void* data, size_t size) {
    std::vector<uint8_t> blob(size);
    if (size > 0) std::memcpy(blob.data(), data, size);
    return blob;
}

void deserialize_binary(const std::vector<uint8_t>& blob, void* out, size_t size) {
    if (blob.size() >= size) std::memcpy(out, blob.data(), size);
}

void write_binary_file(const std::string& path, const std::vector<uint8_t>& data) {
    std::ofstream f(path, std::ios::binary);
    if (f) f.write(reinterpret_cast<const char*>(data.data()),
                   static_cast<std::streamsize>(data.size()));
}

std::vector<uint8_t> read_binary_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return {};
    auto size = f.tellg();
    f.seekg(0);
    std::vector<uint8_t> blob(static_cast<size_t>(size));
    f.read(reinterpret_cast<char*>(blob.data()), size);
    return blob;
}

// ── Simple JSON helpers ──

std::string json_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else out += c;
    }
    return out;
}

} // namespace exd::core
