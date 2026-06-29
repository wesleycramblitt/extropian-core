#include <exd/core/config.hpp>
#include <fstream>
#include <sstream>

namespace exd::core {

void Config::set(const std::string& key, const std::string& value) {
    values_[key] = value;
}

void Config::set_default(const std::string& key, const std::string& value) {
    if (values_.find(key) == values_.end())
        values_[key] = value;
}

std::optional<std::string> Config::get(const std::string& key) const {
    auto it = values_.find(key);
    if (it != values_.end()) return it->second;
    return std::nullopt;
}

bool Config::has(const std::string& key) const {
    return values_.find(key) != values_.end();
}

void Config::load_toml(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        val.erase(0, val.find_first_not_of(" \t\""));
        val.erase(val.find_last_not_of(" \t\"") + 1);
        values_[key] = val;
    }
}

void Config::save_toml(const std::string& path) const {
    std::ofstream f(path);
    if (!f) return;
    for (auto& [k, v] : values_)
        f << k << " = \"" << v << "\"\n";
}

} // namespace exd::core
