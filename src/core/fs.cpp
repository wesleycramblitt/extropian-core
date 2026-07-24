#include <exd/core/fs.hpp>
#include <fstream>
#include <iterator>

namespace exd::core::fs {

std::optional<std::string> read_text_file(std::string_view path) {
    auto path_str = std::string(path);
    std::ifstream f{path_str};
    if (!f) return std::nullopt;
    return std::string(std::istreambuf_iterator<char>(f),
                       std::istreambuf_iterator<char>());
}

std::vector<uint8_t> read_binary_file(std::string_view path) {
    auto path_str = std::string(path);
    std::ifstream f{path_str, std::ios::binary | std::ios::ate};
    if (!f) return {};
    auto sz = f.tellg();
    f.seekg(0);
    std::vector<uint8_t> data(static_cast<size_t>(sz));
    f.read(reinterpret_cast<char*>(data.data()), sz);
    return data;
}

bool write_text_file(std::string_view path, std::string_view content) {
    auto path_str = std::string(path);
    std::ofstream f{path_str};
    if (!f) return false;
    f << content;
    return f.good();
}

bool write_binary_file(std::string_view path,
                       const void* data, size_t size) {
    auto path_str = std::string(path);
    std::ofstream f{path_str, std::ios::binary};
    if (!f) return false;
    f.write(static_cast<const char*>(data),
            static_cast<std::streamsize>(size));
    return f.good();
}

std::vector<std::string> list_directory(std::string_view path) {
    std::vector<std::string> entries;
    std::error_code ec;
    for (const auto& e : std::filesystem::directory_iterator(path, ec))
        entries.push_back(e.path().filename().string());
    return entries;
}

} // namespace exd::core::fs
