#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <cctype>

namespace exd::core::str {

// ────────────────────────────────────────────────────
//  Case conversion
// ────────────────────────────────────────────────────

[[nodiscard]] inline std::string to_lower(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}
[[nodiscard]] inline std::string to_upper(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// ────────────────────────────────────────────────────
//  Trimming
// ────────────────────────────────────────────────────

/// Remove leading and trailing whitespace.
[[nodiscard]] inline std::string trim(std::string s) {
    auto is_space = [](unsigned char c) { return std::isspace(c) != 0; };
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), is_space));
    s.erase(std::find_if_not(s.rbegin(), s.rend(), is_space).base(), s.end());
    return s;
}

/// Remove leading whitespace.
[[nodiscard]] inline std::string trim_left(std::string s) {
    auto is_space = [](unsigned char c) { return std::isspace(c) != 0; };
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), is_space));
    return s;
}

/// Remove trailing whitespace.
[[nodiscard]] inline std::string trim_right(std::string s) {
    auto is_space = [](unsigned char c) { return std::isspace(c) != 0; };
    s.erase(std::find_if_not(s.rbegin(), s.rend(), is_space).base(), s.end());
    return s;
}

// ────────────────────────────────────────────────────
//  Prefix / suffix
// ────────────────────────────────────────────────────

[[nodiscard]] inline bool starts_with(std::string_view s, std::string_view prefix) {
    return s.size() >= prefix.size()
        && s.compare(0, prefix.size(), prefix) == 0;
}

[[nodiscard]] inline bool ends_with(std::string_view s, std::string_view suffix) {
    return s.size() >= suffix.size()
        && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// ────────────────────────────────────────────────────
//  Splitting
// ────────────────────────────────────────────────────

/// Split a string by a delimiter character.
[[nodiscard]] inline std::vector<std::string> split(std::string_view s, char delim) {
    std::vector<std::string> parts;
    size_t start = 0, end = 0;
    while ((end = s.find(delim, start)) != std::string_view::npos) {
        parts.emplace_back(s.substr(start, end - start));
        start = end + 1;
    }
    parts.emplace_back(s.substr(start));
    return parts;
}

/// Split a string by whitespace (one or more spaces/tabs/newlines).
[[nodiscard]] inline std::vector<std::string> split_whitespace(std::string_view s) {
    std::vector<std::string> parts;
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        size_t start = i;
        while (i < s.size() && !std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        if (i > start) parts.emplace_back(s.substr(start, i - start));
    }
    return parts;
}

/// Join a range of strings with a delimiter.
template <typename Range>
[[nodiscard]] std::string join(const Range& parts, std::string_view delim) {
    std::string out;
    bool first = true;
    for (const auto& p : parts) {
        if (!first) out += delim;
        out += p;
        first = false;
    }
    return out;
}

// ────────────────────────────────────────────────────
//  Miscellaneous
// ────────────────────────────────────────────────────

/// Replace all occurrences of `from` with `to`.
[[nodiscard]] inline std::string replace_all(std::string s,
                                              std::string_view from,
                                              std::string_view to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
    return s;
}

/// Check if the string contains a substring.
[[nodiscard]] inline bool contains(std::string_view hay, std::string_view needle) {
    return hay.find(needle) != std::string_view::npos;
}

} // namespace exd::core::str
