# Extropian Core — Foundation Library

> Reusable ECS, math, types, and infrastructure used by every other extropian library.
> No GPU, no audio, no platform dependencies.

## 1. Purpose

Core provides the lowest-level shared infrastructure for the entire ecosystem. Every other extropian library depends on it. It has no dependencies beyond C++23 and standard libraries.

Core answers:

> What reusable building blocks do all extropian libraries need?

Core owns:

- ECS: `Registry`, `Entity`, `View<Cs...>`, `CommandBuffer`, `SystemGraph`, `ISystem`
- **SceneGraph**: `exd::ecs::SceneGraph` — the shared, generic hierarchy over
  entities (parent/child, traversal, reparenting, sibling ordering, lifetime).
  It knows nothing about transforms, rendering, or domain semantics; spatial
  systems combine it with `LocalTransform` components to derive world state.
- **Transforms**: `exd::ecs::LocalTransform` (component state, relative to
  parent) and `exd::ecs::WorldTransform` (derived/consumed state),
  computed by `exd::ecs::TransformSystem` from SceneGraph + LocalTransform.
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

> **Updated 2026-08 — `VisualDocument` is the authored visual document.**
> The cross-language schema now lives in C++ **only** (`include/exd/types/`).
> The TypeScript mirror formerly kept in `extropian-web-ui` is deprecated with
> that repo; the C++ structs are the single authority, and `composer-web`
> consumes the JSON format through WASM. `VisualDocument` is the authored
> document. Resolved scene data and runtime UI state are implementation-owned.

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

## 5.1 VisualDocument — the authored document

`VisualDocument` is the authored, declarative visual document. It describes the
canvas, sections, nodes, relations, data sources, and initial state that make up
the authored visual artifact. Structural edits use `VisualDocumentPatch`.

VisualDocument v1 is deliberately an orthographic 2D authored input contract.
It contains authored shape, geometry, size constraints, and layout intent only.
It excludes 3D/world spaces, cameras, perspective, and resolved x/y/z
transforms; resolved scene data remains implementation-owned rather than being
materialized into authored data.

### 5.2 style_profile — deterministic renderer metrics

`StyleProfile` provides deterministic typography and spacing metrics, so
presentation code can resolve them consistently without per-node baking:

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
    ├── presentation_state.hpp     # StyleOverride, Annotation, AnimationClip, PatchOp
    ├── semantic_document.hpp      # Doc 1: what concepts exist
    ├── visual_document.hpp        # authored visual document
    ├── visual_document_patch.hpp  # structural mutations for VisualDocument
    └── style_profile.hpp           # deterministic renderer spacing/typography metrics
```

Authored VisualDocument styles and `StyleOverride::emphasis` share one
vocabulary (`"subtle | default | primary | prominent"`). Runtime UI overrides
default to `"subtle"` (i.e. dim).

PresentationState is runtime UI state; resolved scene data and related runtime
details are implementation-owned. There is **no TypeScript mirror**; the JSON
contract is C++-only.

## 8. Non-Goals

- No GPU abstraction (extropian-render)
- No audio abstraction (extropian-voice)
- No UI components (spatial-ui's `ui` module)
- No semantic meaning (extropian-composer)
- AI production is outside core; `VisualDocument` is an input contract


## 8. SceneGraph & Transform model (ecosystem architecture)

The ecosystem converges on one model: **entities and components are the
common state; SceneGraph is the common hierarchy; transforms are component
state + derived state**.

- `SceneGraph` (`exd/ecs/scene_graph.hpp`) — a pure containment structure.
  Answers only "what is the hierarchical relationship between these
  entities?". It supports attach/detach/reparent, ordered siblings
  (`attach_at`, `reorder`, `move_after`), DFS traversal with early abort,
  root/depth queries, and generation-safe lifetime handling (`prune()`).
  It does not know about rendering, UI, layout, cameras, geometry, physics,
  or transforms — it is equally useful for spatial trees, CAE part trees,
  and UI widget trees.
- `LocalTransform` (`exd/ecs/transform.hpp`) — an entity's transform
  relative to its parent. Plain component state; no hierarchy semantics.
- `WorldTransform` — derived state: the accumulated local→world transform.
  Written by `TransformSystem` (`exd/ecs/transform_system.hpp`) each update
  from SceneGraph relationships + LocalTransform (TRS accumulation,
  cycle-guarded, generation-safe). Renderers and picking systems consume
  `WorldTransform`; they do not reimplement parent-chain composition.
- Hierarchy and transform are *orthogonal*: a SceneGraph can organize
  non-spatial trees, and entities can carry LocalTransform without a parent.
- `HierarchyIndex` and renderer-local hierarchy components
  (`render::Parent`/`Children`) were removed; SceneGraph + WorldTransform
  is the only hierarchy/transform model.
