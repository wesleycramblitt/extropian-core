// Visual Intent Document (Doc 2) — how to communicate concepts.
//
// ⚠️ LEGACY / DEPRECATED (2026-08). Superseded by `VisualPlan` (Doc 2, see
// visual_plan.hpp). Kept for backward compatibility only.
//
// Part of the 4-document pipeline:
//   Doc 1: Semantic Document (what exists)
//   Doc 2: VisualPlan (semantic composition) ← supersedes this file
//   Doc 3: Scene Document (spaces, nodes, transforms)
//   Doc 4: Presentation State (runtime focus, annotations)
#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace exd
{

// ── Representation Hint ──

struct RepresentationHint
{
    std::string concept_id;
    std::string representation;     // "spatial_transform", "editable_matrix", "equation",
                                    // "draggable_point", "chart", "graph", "diagram",
                                    // "text", "code", "table"
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RepresentationHint, concept_id, representation)

// ── Composition Hint ──

struct CompositionHint
{
    std::string primary;
    std::vector<std::string> supporting;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CompositionHint, primary, supporting)

// ── Top-Level Visual Intent Document ──

struct VisualIntentDocument
{
    std::vector<RepresentationHint> representations;
    CompositionHint composition;
    std::string comparison;                          // "stages", "side_by_side", "overlay"
    bool progressive_disclosure = false;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VisualIntentDocument, representations, composition, comparison, progressive_disclosure)

} // namespace exd
