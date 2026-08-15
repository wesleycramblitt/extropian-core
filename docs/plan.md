# Extropian Core — Foundation Library

> Reusable ECS, math, types, and infrastructure used by every other extropian library.
> No GPU, no audio, no platform dependencies.

## 1. Purpose

Core provides the lowest-level shared infrastructure for the entire ecosystem. Every other extropian library depends on it. It has no dependencies beyond C++23 and standard libraries.

Core answers:

> What reusable building blocks do all extropian libraries need?

Core owns:

- ECS: `Registry`, `Entity`, `View<Cs...>`, `CommandBuffer`, `SystemGraph`, `ISystem`
- Math: `Vec2`, `Vec3`, `Vec4`, `Mat3`, `Mat4`, `Quat`, `DualQuat`, `Bounds3`, `Raycast`, `Color`
- Types: `Handle<T>`, `Vertex`, `PrimitiveTopology`, `MeshData`, `Bounds`
- Utilities: `Signal<T>`, logging, assertions, hash, pool, random, clock, serialization, units
- **WindowState** (abstract interface, moved from extropian-app)

Core does NOT own:

- GPU resources, shaders, or rendering
- Audio capture, playback, or processing
- Window creation or event polling
- Application logic or AI orchestration

## 2. WindowState (NEW in core)

The abstract `WindowState` interface is moved from `extropian-app` into `extropian-core` to break the dependency chain:

```cpp
// core/window_state.hpp
namespace exd::core {

enum class InputMode { FPS = 0, UI = 1 };

struct WindowState {
    virtual ~WindowState() = default;

    virtual void get_dimensions(int& w, int& h, float& aspect) const = 0;
    virtual bool was_key_pressed(int scancode) const = 0;
    virtual bool was_key_released(int scancode) const = 0;
    virtual void reset_mouse_delta() = 0;
    virtual bool mouse_button_down(int button) const = 0;

    InputMode   input_mode      = InputMode::FPS;
    const bool* keyboard_state  = nullptr;
    float       mouse_rel_x     = 0.0f;
    float       mouse_rel_y     = 0.0f;
    float       scroll_x        = 0.0f;
    float       scroll_y        = 0.0f;
    bool        grid_visible    = true;
    bool        wireframe       = false;

    float       gamepad_left_x  = 0.0f;
    float       gamepad_left_y  = 0.0f;
    float       gamepad_right_x = 0.0f;
    float       gamepad_right_y = 0.0f;
    float       gamepad_left_trigger  = 0.0f;
    float       gamepad_right_trigger = 0.0f;
    uint16_t    gamepad_buttons = 0;
};

} // namespace exd::core
```

Both platforms implement this interface:
- **Desktop**: `exd::app::Window` (SDL3-backed, in extropian-app)
- **Web**: Web input handler backed by browser events (in canvas-web)

## 3. ECS Architecture

Single-header archetype-free sparse-set ECS:

```
Registry
├── Entity lifecycle (create/destroy)
├── Component pools (type-erased, sparse set per component)
├── View<Cs...> — iterate entities with specific components
├── CommandBuffer — deferred mutations during iteration
└── SystemGraph — ordered system execution
```

- No archetypes. Components stored in separate sparse sets per type.
- Views iterate the smallest pool and filter others.
- Entities carry a generation counter for safe ID reuse.

## 4. Math Library

GLM-style math with SSE-friendly layout:

| Type | Dimensions | Uses |
|---|---|---|
| `Vec2<T>` | 2 | UV, 2D positions |
| `Vec3<T>` | 3 | Positions, normals, colors |
| `Vec4<T>` | 4 | Homogeneous coords, quaternion parts |
| `Mat3<T>` | 3×3 | Normal transforms |
| `Mat4<T>` | 4×4 | Model/view/proj matrices |
| `Quat<T>` | 4 | Rotations |
| `Bounds3<T>` | min/max | AABB |
| `Raycast<T>` | origin+direction | Picking |

## 5. Types

> **Updated 2026-08 — VisualPlan (Doc 2) added; TS mirror dropped.** The
> cross-language schema now lives in C++ **only** (`include/exd/types/`). The
> TypeScript mirror formerly kept in `extropian-web-ui` is deprecated with that
> repo; the C++ struct is the single authority, and `composer-web` consumes the
> JSON format through WASM. The AI-facing composition document is the new
> **`VisualPlan`** (§5.1), which supersedes `VisualIntent`/`VisualIntentDocument`
> (both now legacy).

```cpp
struct Vertex {
    Vec3f position;
    Vec3f normal;
    Vec3f uv;
    Vec4f tangent;
    Vec4f color;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    PrimitiveTopology topology;
    Bounds bounds;
};
```

## 5.1 VisualPlan (Doc 2) — the semantic composition document

> **New in 2026-08.** `VisualPlan` is the AI-facing composition document. It
> carries **semantics only** — no coordinates, no pixels, no concrete sizes.
> `extropian-semantic-to-visual`'s `VisualPlanCompiler` resolves it
> deterministically into a `SceneDocument`. It supersedes `VisualIntent` /
> `VisualIntentDocument` (both kept, marked `@deprecated`).

The AI decides *what to show and how to emphasize*; the compiler decides *the
geometry*. Target ratio: **1 VisualPlan instruction → 10–100 scene operations**.

```cpp
// include/exd/types/visual_plan.hpp
namespace exd {

enum class Density { Spacious, Standard, Dense, Reference, Poster, Presentation }; // lowercase on wire

struct HeroHint {                          // the single dominant visual
    std::string ref;                       // entity id
    std::optional<std::string> form;       // grammar id, e.g. "layer_stack"
};

struct Section {                           // semantic topology, not pixels
    std::string id;
    std::string strategy;                  // L3 grammar id (see visual-grammar.md)
    std::vector<std::string> entities;     // entity ids to include
    std::string emphasis = "default";      // subtle | default | primary | prominent
};

struct Entity {                            // semantic content graph node
    std::string id;
    std::string label;
    std::optional<std::string> kind;       // free-form semantic kind
    float importance = 0.5f;               // 0..1 → priority engine input
};

struct RepresentationChoice {              // one entity, multiple encodings
    std::string kind;                      // grammar id (equation_fragment, grid_stencil, field_curvature, …)
    std::string role = "primary";          // primary | intuition | physical_effect | reference | …
};

struct Representation {
    std::string semanticRef;               // entity id
    std::vector<RepresentationChoice> representations;
};

struct PlanRelation {                      // constraint-based, not coordinates
    std::string kind;                      // connect | right_of | left_of | above | below | inside | flows | causes | …
    std::string source;
    std::string target;
    std::optional<std::string> from_port;  // "east" etc. (for connect)
    std::optional<std::string> to_port;
};

struct PlanOverride {                      // controlled deviations — constraints, not coords
    std::string op;                        // attach | emphasize | deemphasize | …
    std::string target;
    std::optional<std::string> visual;     // grammar to attach (attach)
    std::optional<std::string> position;   // "semantic_near" (attach)
};

struct PriorityInputs {                    // optional AI inputs → deterministic scoring
    float relevance = 0.5f;
    float focus_proximity = 0.5f;
    float novelty = 0.5f;
    float dependency_value = 0.5f;
};

struct VisualPlan {
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
```

The 22 L3 grammars (`AnnotatedEquation` … `OptimizationLandscape`) and the 10
macro strategies (`hero_support` … `reference_sheet`) are specified in
`extropian-semantic-to-visual/compiler/docs/visual-grammar.md` §8/§9.

### 5.2 style_profile — deterministic density resolution

`VisualPlan.density` resolves into a `style_profile` that the `VisualPlanCompiler`
embeds in the emitted `SceneDocument` (additive optional field), so any renderer
resolves typography/spacing/geometry consistently without per-node baking:

```cpp
// include/exd/types/style_profile.hpp
namespace exd {

struct StyleProfile {
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
```

Semantic style tokens (`semantic.variable`, `semantic.operator`,
`semantic.parameter`, `semantic.source`, `semantic.input`, `semantic.output`,
`semantic.warning`, `semantic.selected`, `semantic.reference`) and typography
roles (`title`, `sectionHeading`, `heroEquation`, `equation`, `body`, `caption`,
`annotation`, `microLabel`) are resolved by `extropian-spatial-ui`'s style
resolver against the active profile — never as raw RGB in the document.

## 6. Platform & Compilation

- Compiles on any C++23 compiler (Clang, GCC, MSVC)
- Compiles to WASM via Emscripten with zero changes
- No platform-specific headers, no GPU, no audio
- No dependencies beyond the C++ standard library

## 7. File Layout

```
include/exd/
├── core/           # namespace exd::core
│   ├── namespace.hpp
│   ├── logging.hpp, assert.hpp, hash.hpp
│   ├── string_util.hpp, pool.hpp
│   ├── clock.hpp, random.hpp
│   ├── serialization.hpp, units.hpp
│   ├── allocator.hpp, fs.hpp
│   ├── mesh_types.hpp, handle.hpp
│   └── window_state.hpp
├── ecs/             # namespace exd::ecs
│   ├── entity.hpp, component.hpp
│   ├── registry.hpp, view.hpp
│   ├── command_buffer.hpp
│   ├── system_graph.hpp, system.hpp
├── math/            # namespace exd::math
│   ├── vec2.hpp, vec3.hpp, vec4.hpp
│   ├── mat3.hpp, mat4.hpp
│   ├── quat.hpp, bounds.hpp
│   ├── raycast.hpp, color.hpp     # ColorRGB / ColorRGBA
└── types/           # cross-language schema structs (C++ authority — no TS mirror)
    ├── scene_document.hpp         # SceneDocument (+style_profile, +composition), NodeStyle, NodeInteraction
    ├── presentation_state.hpp     # StyleOverride, Annotation, AnimationClip, PatchOp
    ├── semantic_document.hpp      # Doc 1: what concepts exist
    ├── visual_plan.hpp            # Doc 2: semantic composition (VisualPlan)
    ├── style_profile.hpp          # density → deterministic spacing/typography metrics
    ├── visual_intent.hpp          # legacy (deprecated)
    └── visual_intent_document.hpp # legacy (deprecated)
```

**Resolved — `emphasis` vocabulary.** Both structs in `types/` now share one
vocabulary (`"subtle | default | primary | prominent"`):
- `scene_document.hpp`'s `NodeStyle::emphasis`
- `presentation_state.hpp`'s `StyleOverride::emphasis` (default `"subtle"`, i.e. dim)

A third vocabulary (`"subtle | moderate | prominent"`) still appears in the AI
system prompt in `extropian-composer/composer.toml` — map `"moderate"` to
`"default"`/`"primary"` when generating. `extropian-spatial-ui`'s
`scene_renderer.cpp` reads only `NodeStyle::emphasis`, which is unaffected.

**SceneDocument additions (additive, 2026-08).** `SceneDocument` gains two
optional fields: `style_profile` (`StyleProfile`, §5.2) and `composition`
(string, informational macro-strategy id). Everything else is unchanged, so
existing documents remain valid. There is **no TypeScript mirror** — the JSON
contract is C++-only; `composer-web` consumes it through WASM.

## 8. Non-Goals

- No GPU abstraction (extropian-render)
- No audio abstraction (extropian-voice)
- No UI components (spatial-ui's `ui` module)
- No semantic meaning (extropian-composer)
- No AI orchestration (extropian-semantic-to-visual/conductor)
