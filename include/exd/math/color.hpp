#pragma once

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <type_traits>

namespace exd::math {

// ────────────────────────────────────────────────────
//  ColorRGB — 3-channel linear RGB (floating point)
// ────────────────────────────────────────────────────

struct ColorRGB {
    float r = 0.0f, g = 0.0f, b = 0.0f;

    [[nodiscard]] bool operator==(const ColorRGB& o) const = default;

    /// Scale by a scalar.
    [[nodiscard]] ColorRGB operator*(float s) const { return {r * s, g * s, b * s}; }
    [[nodiscard]] ColorRGB operator/(float s) const { return {r / s, g / s, b / s}; }
    ColorRGB& operator*=(float s) { r *= s; g *= s; b *= s; return *this; }

    /// Add two colors (useful for light accumulation).
    [[nodiscard]] ColorRGB operator+(const ColorRGB& o) const { return {r + o.r, g + o.g, b + o.b}; }
    ColorRGB& operator+=(const ColorRGB& o) { r += o.r; g += o.g; b += o.b; return *this; }

    /// Modulate (component-wise multiply).
    [[nodiscard]] ColorRGB operator*(const ColorRGB& o) const { return {r * o.r, g * o.g, b * o.b}; }

    /// Clamp all channels to [0, 1].
    [[nodiscard]] ColorRGB saturate() const {
        return {std::clamp(r, 0.0f, 1.0f),
                std::clamp(g, 0.0f, 1.0f),
                std::clamp(b, 0.0f, 1.0f)};
    }

    /// sRGB → linear (approximate gamma 2.2).
    [[nodiscard]] ColorRGB srgb_to_linear() const;

    /// Linear → sRGB (approximate gamma 2.2).
    [[nodiscard]] ColorRGB linear_to_srgb() const;

    // Common colors
    static ColorRGB black()  { return {0, 0, 0}; }
    static ColorRGB white()  { return {1, 1, 1}; }
    static ColorRGB red()    { return {1, 0, 0}; }
    static ColorRGB green()  { return {0, 1, 0}; }
    static ColorRGB blue()   { return {0, 0, 1}; }
    static ColorRGB cyan()   { return {0, 1, 1}; }
    static ColorRGB magenta(){ return {1, 0, 1}; }
    static ColorRGB yellow() { return {1, 1, 0}; }
};

// ────────────────────────────────────────────────────
//  ColorRGBA — 4-channel linear RGBA (floating point)
// ────────────────────────────────────────────────────

struct ColorRGBA {
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;

    [[nodiscard]] bool operator==(const ColorRGBA& o) const = default;

    [[nodiscard]] ColorRGBA operator*(float s) const { return {r * s, g * s, b * s, a * s}; }
    ColorRGBA& operator*=(float s) { r *= s; g *= s; b *= s; a *= s; return *this; }

    [[nodiscard]] ColorRGBA operator+(const ColorRGBA& o) const { return {r + o.r, g + o.g, b + o.b, a + o.a}; }
    ColorRGBA& operator+=(const ColorRGBA& o) { r += o.r; g += o.g; b += o.b; a += o.a; return *this; }

    /// Modulate (component-wise multiply).
    [[nodiscard]] ColorRGBA operator*(const ColorRGBA& o) const { return {r * o.r, g * o.g, b * o.b, a * o.a}; }

    /// Extract RGB portion.
    [[nodiscard]] ColorRGB rgb() const { return {r, g, b}; }

    /// Construct from float RGBA.
    static ColorRGBA from_u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return {r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
    }

    /// Construct from 0xRRGGBBAA.
    static ColorRGBA from_hex(uint32_t hex) {
        return from_u8((hex >> 24) & 0xFF,
                       (hex >> 16) & 0xFF,
                       (hex >> 8)  & 0xFF,
                       (hex >> 0)  & 0xFF);
    }

    /// Convert floating-point [0,1] to uint8_t.
    [[nodiscard]] uint32_t to_hex() const {
        auto sat = saturate();
        return (static_cast<uint32_t>(sat.r * 255) << 24)
             | (static_cast<uint32_t>(sat.g * 255) << 16)
             | (static_cast<uint32_t>(sat.b * 255) << 8)
             | (static_cast<uint32_t>(sat.a * 255) << 0);
    }

    [[nodiscard]] ColorRGBA saturate() const {
        return {std::clamp(r, 0.0f, 1.0f),
                std::clamp(g, 0.0f, 1.0f),
                std::clamp(b, 0.0f, 1.0f),
                std::clamp(a, 0.0f, 1.0f)};
    }

    [[nodiscard]] ColorRGBA srgb_to_linear() const;
    [[nodiscard]] ColorRGBA linear_to_srgb() const;

    static ColorRGBA black()   { return {0, 0, 0, 1}; }
    static ColorRGBA white()   { return {1, 1, 1, 1}; }
    static ColorRGBA red()     { return {1, 0, 0, 1}; }
    static ColorRGBA green()   { return {0, 1, 0, 1}; }
    static ColorRGBA blue()    { return {0, 0, 1, 1}; }
    static ColorRGBA transparent() { return {0, 0, 0, 0}; }
};

// ────────────────────────────────────────────────────
//  sRGB ↔ linear helpers (inlined for LTO benefit)
// ────────────────────────────────────────────────────

namespace detail {
    inline float srgb_to_linear_scalar(float c) {
        if (c <= 0.04045f) return c / 12.92f;
        return std::pow((c + 0.055f) / 1.055f, 2.4f);
    }
    inline float linear_to_srgb_scalar(float c) {
        if (c <= 0.0031308f) return c * 12.92f;
        return 1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f;
    }
} // namespace detail

inline ColorRGB ColorRGB::srgb_to_linear() const {
    return {detail::srgb_to_linear_scalar(r),
            detail::srgb_to_linear_scalar(g),
            detail::srgb_to_linear_scalar(b)};
}
inline ColorRGB ColorRGB::linear_to_srgb() const {
    return {detail::linear_to_srgb_scalar(r),
            detail::linear_to_srgb_scalar(g),
            detail::linear_to_srgb_scalar(b)};
}
inline ColorRGBA ColorRGBA::srgb_to_linear() const {
    return {detail::srgb_to_linear_scalar(r),
            detail::srgb_to_linear_scalar(g),
            detail::srgb_to_linear_scalar(b), a};
}
inline ColorRGBA ColorRGBA::linear_to_srgb() const {
    return {detail::linear_to_srgb_scalar(r),
            detail::linear_to_srgb_scalar(g),
            detail::linear_to_srgb_scalar(b), a};
}

} // namespace exd::math
