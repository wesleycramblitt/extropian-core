// Presentation State (Doc 4) — runtime focus, annotations, patches.
//
// Part of the 4-document pipeline:
//   Doc 1: Semantic Document (what exists)
//   Doc 2: Visual Intent Document (how to communicate)
//   Doc 3: Scene Document (spaces, nodes, transforms)
//   Doc 4: Presentation State (runtime focus, annotations, animations) ← this file
//
// This is the runtime patch layer. The AI emits PatchDocuments to modify
// the current presentation without regenerating the full scene.
#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

// Enable std::optional<T> serialization in NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE macros
#include <exd/core/json_optional.hpp>

namespace exd
{

// ── Camera Override ──

struct CameraPose_ps
{
    std::array<float, 3> look_at = {0, 0, 5};
    std::array<float, 3> target  = {0, 0, 0};
    std::array<float, 3> up      = {0, 1, 0};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraPose_ps, look_at, target, up)

struct CameraOverride
{
    std::string space;                       // which space's camera to modify
    std::optional<CameraPose_ps> pose;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraOverride, space, pose)

// ── Style Override ──

struct StyleOverride
{
    // Same emphasis vocabulary as NodeStyle::emphasis (scene_document.hpp):
    // "subtle" | "default" | "primary" | "prominent". Runtime overrides default
    // to "subtle" (dim) and are usually paired with a low opacity.
    std::string emphasis = "subtle";
    float opacity = 0.15f;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StyleOverride, emphasis, opacity)

// ── Annotation ──

struct Annotation
{
    std::string id;
    std::string target;                      // node id
    std::string text;
    std::string position = "below";          // above | below | left | right | center
    std::string style = "callout";           // callout | tooltip | label
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Annotation, id, target, text, position, style)

// ── Animation Clip ──

struct AnimationClip
{
    std::string target;                      // node id
    std::string effect;                      // pulse | highlight | fade_in | fade_out | slide_in | scale_up
    float duration = 1.5f;
    std::string easing = "ease_out";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationClip, target, effect, duration, easing)

// ── Top-Level Presentation State ──

struct PresentationState
{
    std::optional<std::string> focus_entity;
    std::vector<std::string> selection;
    std::optional<CameraOverride> camera;
    std::map<std::string, StyleOverride> overrides;      // node_id -> override
    std::vector<Annotation> annotations;
    std::vector<AnimationClip> animations;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PresentationState, focus_entity, selection, camera, overrides, annotations, animations)

// ── Patch Operations (runtime AI mutations) ──

struct PatchOp
{
    std::string op;                          // isolate | camera_focus | annotate | dim |
                                             // highlight | reveal | conceal | scrub | sequence | reset
    std::string target;                      // node id
    nlohmann::json params;                   // op-specific: {text, position, duration, from, to, ...}
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PatchOp, op, target, params)

struct PatchDocument
{
    std::vector<PatchOp> ops;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PatchDocument, ops)

} // namespace exd
