#pragma once
#include <optional>
#include <string_view>
namespace exd {
/// Resolved layout size vocabulary ("auto", "fill", px, %, rem).
enum class SizeSpecKind { Auto, Fill, Px, Rem, Percent };
struct SizeSpec { SizeSpecKind kind = SizeSpecKind::Auto; float value = 0.0f; };
/// Parse a width/height/min/max string:
///   "" | "auto"          → Auto
///   "fill"               → Fill
///   "<number>"           → Px
///   "<number>%"          → Percent
///   "<number>rem"        → Rem   (scaling to px is the caller's job)
/// Returns nullopt for any other text (callers fall back to Auto).
std::optional<SizeSpec> parse_size_spec(std::string_view text);
}
