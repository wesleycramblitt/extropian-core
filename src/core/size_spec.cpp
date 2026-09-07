#include <exd/types/size_spec.hpp>
#include <cctype>
#include <string>

namespace exd {

namespace {

// Trim leading/trailing ASCII whitespace from a view.
std::string_view trim(std::string_view s)
{
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front())))
        s.remove_prefix(1);
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))
        s.remove_suffix(1);
    return s;
}

// Parse an unsigned decimal number ("12", "8.5", "0.5"). Returns false on
// any malformed input (empty, non-digit, trailing junk, negative).
bool parse_number(std::string_view s, float& out)
{
    if (s.empty())
        return false;

    size_t i = 0;
    if (s[i] == '-')
        return false; // negatives rejected

    bool any_digit = false;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
        any_digit = true;
        ++i;
    }
    if (i < s.size() && s[i] == '.') {
        ++i;
        while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
            any_digit = true;
            ++i;
        }
    }
    if (!any_digit || i != s.size())
        return false;

    out = std::stof(std::string(s));
    return true;
}

} // namespace

std::optional<SizeSpec> parse_size_spec(std::string_view text)
{
    const std::string_view s = trim(text);

    if (s.empty() || s == "auto")
        return SizeSpec{SizeSpecKind::Auto, 0.0f};
    if (s == "fill")
        return SizeSpec{SizeSpecKind::Fill, 0.0f};

    // <number>%
    if (!s.empty() && s.back() == '%') {
        float v;
        if (parse_number(s.substr(0, s.size() - 1), v))
            return SizeSpec{SizeSpecKind::Percent, v};
        return std::nullopt;
    }

    // <number>rem
    static constexpr std::string_view rem = "rem";
    if (s.size() > rem.size() &&
        s.substr(s.size() - rem.size()) == rem) {
        float v;
        if (parse_number(s.substr(0, s.size() - rem.size()), v))
            return SizeSpec{SizeSpecKind::Rem, v};
        return std::nullopt;
    }

    // <number>
    float v;
    if (parse_number(s, v))
        return SizeSpec{SizeSpecKind::Px, v};

    return std::nullopt;
}

} // namespace exd
