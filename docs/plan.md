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
└── types/           # cross-language schema structs (C++ + TS authority)
    ├── scene_document.hpp         # SceneDocument, NodeStyle, NodeInteraction
    ├── presentation_state.hpp     # StyleOverride and friends
    ├── semantic_document.hpp
    ├── visual_intent.hpp
    └── visual_intent_document.hpp
```

**Resolved — `emphasis` vocabulary.** Both structs in `types/` now share one
vocabulary (`"subtle | default | primary | prominent"`):
- `scene_document.hpp`'s `NodeStyle::emphasis`
- `presentation_state.hpp`'s `StyleOverride::emphasis` (default `"subtle"`, i.e. dim)

A third vocabulary (`"subtle | moderate | prominent"`) still appears in the AI
system prompt in `extropian-composer/composer.toml` — map `"moderate"` to
`"default"`/`"primary"` when generating. `extropian-spatial-ui`'s
`scene_renderer.cpp` reads only `NodeStyle::emphasis`, which is unaffected.

## 8. Non-Goals

- No GPU abstraction (extropian-render)
- No audio abstraction (extropian-voice)
- No UI components (spatial-ui's `ui` module)
- No semantic meaning (extropian-composer)
- No AI orchestration (extropian-semantic-to-visual/conductor)
