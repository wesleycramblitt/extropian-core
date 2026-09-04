// Style Profile — deterministic renderer typography and spacing metrics.
//
// A StyleProfile provides deterministic typography and spacing metrics without
// baking those presentation details into authored document nodes.
//
// Semantic style tokens (`semantic.variable`, `semantic.operator`,
// `semantic.parameter`, `semantic.source`, `semantic.input`, `semantic.output`,
// `semantic.warning`, `semantic.selected`, `semantic.reference`) and typography
// roles (`title`, `sectionHeading`, `heroEquation`, `equation`, `body`,
// `caption`, `annotation`, `microLabel`) are resolved by `extropian-spatial-ui`'s
// style resolver against the active profile — never as raw RGB in the document.
#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace exd
{

// Density → deterministic spacing/typography metrics.
// The `density` string uses lowercase values on the wire:
//   spacious | standard | dense | reference | poster | presentation
struct StyleProfile
{
    std::string density = "standard";      // spacious | standard | dense | reference | poster | presentation
    float base_font = 14.0f;
    float caption_font = 10.0f;
    float panel_gap = 16.0f;
    float section_gap = 12.0f;
    float padding = 16.0f;
    float annotation_gap = 4.0f;
    float border_width = 1.0f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StyleProfile, density, base_font, caption_font,
    panel_gap, section_gap, padding, annotation_gap, border_width)

} // namespace exd
