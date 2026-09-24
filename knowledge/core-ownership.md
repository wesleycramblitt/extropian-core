# Core Ownership

Core answers: what reusable building blocks do all extropian libraries need.

Owns (`include/exd/`, `src/`):

- `exd::ecs` — `Registry`, `Entity`, `View<Cs...>`, `CommandBuffer`, `SystemGraph`, `ISystem`, `SceneGraph`, `Local/WorldTransform`, `TransformSystem`.
- `exd::math` — `Vec2/3/4`, `Mat3/4`, `Quat`, `DualQuat`, `Bounds3`, `Raycast`, `Color`.
- `exd::core` — `Config`, logging, units, `LinearAllocator`, serialization, event bus, plugin loader, `WindowState`, debug macros, `Handle<T>`, `MeshData`/`Vertex`, `Bounds`.
- `exd::types` — JSON-serializable schema structs (C++ authority only).

Does NOT own:

- GPU resources, shaders, rendering; audio; window creation / event polling; app logic / AI orchestration.

## Namespaces and entry point

- `exd::math`, `exd::ecs`, `exd::core`, `exd::types` (`README.md`, `CMakeLists.txt` project `extropian-core`).
- Umbrella: `#include <exd/ext.hpp>` pulls in all public modules.

## WindowState contract

Abstract platform-agnostic input/display interface, moved from `extropian-app` to break the dependency chain (`include/exd/core/window_state.hpp` is authority; `docs/plan.md` §2 example is superseded):

- Pure virtuals: `get_dimensions`, `was_key_pressed/released`, `reset_mouse_delta`, `mouse_button_down`.
- Defaulted virtuals: `get_drawable_size` (device pixels for `glViewport`/scissor; falls back to logical size), `set_input_mode`, `native_handle` / `native_gl_context` (SDL3 handles on desktop, `nullptr` on web).
- State fields: `input_mode` defaults to `InputMode::UI` (select mouse; apps opt into FPS explicitly), `keyboard_state`, `mouse_rel_x/y`, `cursor_x/y` (top-left origin), `scroll_x/y`, `grid_visible`, `wireframe`, gamepad sticks/triggers/buttons.
- `CursorMode::{Normal,Captured}` and `WindowEvents::{on_resize,on_focus,on_minimize,on_close}` live alongside.
- Implementations: desktop `exd::app::Window` (SDL3, in extropian-app); web browser-event handler.

Sources: `docs/plan.md` §§1–2,6; `include/exd/core/window_state.hpp`; `include/exd/ext.hpp`; commits `c0b7063` (default UI), `022d94d` (`get_drawable_size`).
