#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <sstream>

namespace ext::core {

/// @brief Simple key-value configuration system with typed getters.
///
/// Read from TOML/JSON files or command-line args. Used by ext::app::Application.
class Config {
public:
    Config() = default;

    /// ── Setters ──────────────────────────────────────
    void set(const std::string& key, const std::string& value);
    void set_default(const std::string& key, const std::string& value);

    template <typename T>
    void set(const std::string& key, const T& value) {
        std::ostringstream oss;
        oss << value;
        set(key, oss.str());
    }

    /// ── Getters ──────────────────────────────────────
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

    /// ── File I/O ─────────────────────────────────────
    void load_toml(const std::string& path);
    void save_toml(const std::string& path) const;

private:
    std::unordered_map<std::string, std::string> values_;
};

} // namespace ext::core
