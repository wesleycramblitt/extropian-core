#pragma once

#include <string>
#include <optional>

namespace exd::core {

class Config {
public:
    Config() = default;
    void set(const std::string& key, const std::string& value);
    void set_default(const std::string& key, const std::string& value);

    template <typename T>
    void set(const std::string& key, const T& value) {
        set(key, std::to_string(value));
    }

    [[nodiscard]] std::optional<std::string> get(const std::string& key) const;

    template <typename T>
    [[nodiscard]] T get_or(const std::string& key, T default_val) const {
        auto opt = get(key);
        if (!opt) return default_val;
        T val{};
        std::istringstream iss(*opt);
        iss >> val;
        return val;
    }

    [[nodiscard]] bool has(const std::string& key) const;
    void load_toml(const std::string& path);
    void save_toml(const std::string& path) const;

private:
    std::unordered_map<std::string, std::string> values_;
};

} // namespace exd::core
