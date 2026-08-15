// VisualPlan (Doc 2) — the semantic composition document (§5.1).
//
// `VisualPlan` is the AI-facing composition document. It carries **semantics
// only** — no coordinates, no pixels, no concrete sizes.
// `extropian-semantic-to-visual`'s `VisualPlanCompiler` resolves it
// deterministically into a `SceneDocument`. It supersedes `VisualIntent` /
// `VisualIntentDocument` (both kept, marked `@deprecated`).
//
// The AI decides *what to show and how to emphasize*; the compiler decides *the
// geometry*. Target ratio: **1 VisualPlan instruction → 10–100 scene operations**.
//
// The 22 L3 grammars (`AnnotatedEquation` … `OptimizationLandscape`) and the 10
// macro strategies (`hero_support` … `reference_sheet`) are specified in
// `extropian-semantic-to-visual/compiler/docs/visual-grammar.md` §8/§9.
#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

// Enable std::optional<T> serialization in NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macros
#include <exd/core/json_optional.hpp>

namespace exd
{

// Composition density — resolves into a style_profile. Lowercase on wire.
enum class Density
{
    Spacious,
    Standard,
    Dense,
    Reference,
    Poster,
    Presentation
};
// Serialize as lowercase strings to match the wire format.
NLOHMANN_JSON_SERIALIZE_ENUM(Density, {
    {Density::Spacious,     "spacious"},
    {Density::Standard,     "standard"},
    {Density::Dense,        "dense"},
    {Density::Reference,    "reference"},
    {Density::Poster,       "poster"},
    {Density::Presentation, "presentation"},
})

// The single dominant visual.
struct HeroHint
{
    std::string ref;                       // entity id
    std::optional<std::string> form;       // grammar id, e.g. "layer_stack"
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeroHint, ref, form)

// Semantic topology, not pixels.
struct Section
{
    std::string id;
    std::string strategy;                  // L3 grammar id (see visual-grammar.md)
    std::vector<std::string> entities;     // entity ids to include
    std::string emphasis = "default";      // subtle | default | primary | prominent
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Section, id, strategy, entities, emphasis)

// Semantic content graph node.
struct Entity
{
    std::string id;
    std::string label;
    std::optional<std::string> kind;       // free-form semantic kind
    float importance = 0.5f;               // 0..1 → priority engine input
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Entity, id, label, kind, importance)

// One entity, multiple encodings.
struct RepresentationChoice
{
    std::string kind;                      // grammar id (equation_fragment, grid_stencil, field_curvature, …)
    std::string role = "primary";          // primary | intuition | physical_effect | reference | …
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RepresentationChoice, kind, role)

struct Representation
{
    std::string semanticRef;               // entity id
    std::vector<RepresentationChoice> representations;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Representation, semanticRef, representations)

// Constraint-based, not coordinates.
struct PlanRelation
{
    std::string kind;                      // connect | right_of | left_of | above | below | inside | flows | causes | …
    std::string source;
    std::string target;
    std::optional<std::string> from_port;  // "east" etc. (for connect)
    std::optional<std::string> to_port;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlanRelation, kind, source, target, from_port, to_port)

// Controlled deviations — constraints, not coords.
struct PlanOverride
{
    std::string op;                        // attach | emphasize | deemphasize | …
    std::string target;
    std::optional<std::string> visual;     // grammar to attach (attach)
    std::optional<std::string> position;   // "semantic_near" (attach)
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlanOverride, op, target, visual, position)

// Optional AI inputs → deterministic scoring.
struct PriorityInputs
{
    float relevance = 0.5f;
    float focus_proximity = 0.5f;
    float novelty = 0.5f;
    float dependency_value = 0.5f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PriorityInputs, relevance, focus_proximity, novelty, dependency_value)

// The AI-facing semantic composition document.
struct VisualPlan
{
    std::string version = "0.1";
    std::string topic;
    Density density = Density::Standard;   // → style_profile
    std::string composition;               // macro strategy id (see visual-grammar.md §8)
    std::optional<HeroHint> hero;
    std::vector<Section> sections;
    std::vector<Entity> entities;
    std::vector<Representation> representations;
    std::vector<PlanRelation> relations;
    std::vector<PlanOverride> overrides;
    std::optional<PriorityInputs> priority;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VisualPlan, version, topic, density, composition,
    hero, sections, entities, representations, relations, overrides, priority)

} // namespace exd
