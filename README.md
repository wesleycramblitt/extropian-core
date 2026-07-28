# extropian-core

**Foundation library for all Extropian projects.**

Zero dependencies beyond C++23 and the standard library. Provides math types, an ECS framework, and core utilities every Extropian library and application builds upon.

## Contents

| Module | What It Provides |
|--------|-----------------|
| `exd::math` | Vec2, Vec3, Vec4, Mat3, Mat4, Quat, DualQuat, transforms, Bounds3, Raycast, Color |
| `exd::ecs` | Entity, Registry, Component concept, ISystem, CommandBuffer, SystemGraph, sparse-set views |
| `exd::core` | Config, logging, units, allocators, serialization, event bus, plugin loader, WindowState, debug macros |

---

## exd::math — Linear Algebra & Transforms

All math types are header-only, templated where appropriate, and live in `include/exd/math/`. An umbrella header `<exd/math/math.hpp>` pulls in everything.

### Vec2\<T\>, Vec3\<T\>, Vec4\<T\>

Template vector types with full arithmetic and geometric operations.

```cpp
#include <exd/math/vec3.hpp>
using namespace exd::math;

Vec3f a{1, 2, 3};

// Arithmetic
Vec3f b = a + Vec3f{4, 5, 6};   // per-component
Vec3f c = a * 2.0f;              // scalar multiply
Vec3f d = -a;                    // negation

// Compound assignment
a += Vec3f{1, 0, 0};
a *= 3.0f;

// Vector operations
float dot = a.dot(b);
Vec3f cross = a.cross(b);
float len = a.length();
float len2 = a.length_sq();
Vec3f n = a.normalized();        // returns zero vector for zero input
float dist = a.distance(b);

// Element access
float x = a[0];                  // same as a.x
a[1] = 5.0f;                     // same as a.y = 5.0f
const float* ptr = a.data();     // raw float[3] pointer

// Static helpers
Vec3f::zero();
Vec3f::one();
Vec3f::up();                      // (0,1,0)
Vec3f::forward();                 // (0,0,-1) — negative Z is forward
Vec3f::right();                   // (1,0,0)

// Scalar-left multiply
Vec3f s = 2.0f * Vec3f{1, 2, 3};

// Aliases
using Vec2f = Vec2<float>;        // also Vec2d, Vec2i
using Vec3f = Vec3<float>;        // also Vec3d, Vec3i
using Vec4f = Vec4<float>;        // also Vec4d, Vec4i
```

**Vec2 extras:** `cross()` returns scalar (2D wedge product), `up()` → (0,1), `right()` → (1,0).  
**Vec4 extras:** `xyz()` → pointer to first 3 components for homogeneous-coordinate convenience.

### Mat3 — 3×3 Column-Major Matrix

Represents linear transforms (rotation, scale, shear) without translation.

```cpp
#include <exd/math/mat3.hpp>

auto I = Mat3::identity();
auto Z = Mat3::zero();

auto R = Mat3::mul(A, B);           // matrix multiplication
auto T = Mat3::transpose(M);        // transpose
float d = Mat3::det(M);             // determinant
auto inv = Mat3::inverse(M);        // general inverse (singular → NaN)

auto rot = Mat3::from_axis_angle(Vec3f{0,0,1}, M_PI/2);  // rotation matrix
auto S = Mat3::from_scale(Vec3f{2, 3, 1});               // diagonal scale
auto M3 = Mat3::from_mat4(mat4_ptr);                     // extract upper-left 3×3 from Mat4

Vec3f v = Mat3::mul_vec(rot, Vec3f{1, 0, 0});            // M * v
```

### Mat4 — 4×4 Column-Major Matrix

Homogeneous transforms with built-in projection and camera utilities.

```cpp
#include <exd/math/mat4.hpp>

auto I = Mat4::identity();

auto P = Mat4::perspective(fov_rad, aspect, znear, zfar);
auto V = Mat4::look_at(eye_pos, look_at_target, world_up);

// Model matrix: Translation * Rotation * Scale
auto M = Mat4::trs(Vec3f{1,2,3}, rotation_quat, Vec3f{2,2,2});

// Model matrix with skew (T * R * K * S)
auto Ms = Mat4::trs(pos, rot, scale, Vec3f{skew_xy, skew_xz, skew_yz});

auto combined = Mat4::mul(P, Mat4::mul(V, M));           // P * V * M
```

### Quat — Rotation Quaternion

Unit quaternion for compact, singularity-free rotation representation.

```cpp
#include <exd/math/quat.hpp>

Quat id{1, 0, 0, 0};                                       // identity

auto q = Quat::from_axis_angle(Vec3f{1,0,0}, 0.5f);       // axis + angle (radians)

Vec3f right   = q.right();                                  // local X axis
Vec3f up      = q.up();                                     // local Y axis
Vec3f forward = q.forward();                                // local Z axis

auto n = q.norm();                                          // normalize to unit length

Vec3f rotated = q * Vec3f{1, 0, 0};                        // rotate a vector

auto q2 = Quat::from_rotation_matrix(mat4_ptr);            // extract from matrix

Quat product = q1 * q2;                                    // compose rotations
Quat sum = q1 + q2;                                        // component-wise (for blending)
```

### DualQuat — Dual Quaternion for Rigid Transforms

Combined rotation + translation with smooth interpolation (scLERP). Essential for skeletal animation.

```cpp
#include <exd/math/dual_quat.hpp>

auto id  = DualQuat::identity();
auto rot = DualQuat::from_rotation(quat);
auto tr  = DualQuat::from_translation(Vec3f{10, 0, 0});
auto dq  = DualQuat::from_pose(quat, Vec3f{1, 2, 3});     // rotation + translation

// Decompose
Quat r_out; Vec3f t_out;
dq.decompose(r_out, t_out);                                 // round-trip: from_pose → decompose

// Compose (applies b then a)
auto composed = a * b;

// Transform a point
Vec3f p = dq.transform(Vec3f{0, 0, 0});

// Interpolation
auto mid = DualQuat::sclerp(a, b, 0.5f);                   // screw-linear, t in [0,1]

// Convert to matrix
float m[16];
dq.to_mat4(m);
```

---

## exd::ecs — Entity Component System

Headers in `include/exd/ecs/`. Core types:
- `entity.hpp` — Entity handle with generation counter
- `registry.hpp` — sparse-set component storage and lifecycle
- `view.hpp` — View<Cs...> template for iterating entities with components
- `component.hpp` — Component concept and type traits
- `system.hpp` — ISystem abstract base
- `command_buffer.hpp` — deferred mutation queue
- `system_graph.hpp` — ordered system execution

The source file `src/ecs/registry.cpp` contains entity lifecycle and introspection.

### Entity

```cpp
struct Entity {
    using id_type = uint32_t;
    using gen_type = uint32_t;

    id_type id;      // unique index, recycled from free list
    gen_type gen;    // generation counter, incremented on destroy
    std::string name;
};

// Equality compares id + gen only (name is metadata)
bool same = (a == b);
```

### Registry — Sparse-Set Component Storage

```cpp
#include <exd/ecs/registry.hpp>
using namespace exd::ecs;

Registry reg;

// Lifecycle
Entity e = reg.create("player");         // allocates entity, returns handle
bool ok   = reg.valid(e);               // true while entity is alive
reg.destroy(e);                          // invalidates entity, frees components, recycles id
reg.clear();                             // destroys all entities and pools

// Introspection
auto all    = reg.all_entities();        // std::vector<Entity> of all alive entities
size_t n    = reg.entity_count();        // count of alive entities

// Component operations
auto& pos = reg.emplace<Position>(e, 1.0f, 2.0f, 3.0f);  // add or replace component
auto& p   = reg.get<Position>(e);                          // throws if missing
auto* p   = reg.try_get<Position>(e);                      // nullptr if missing
bool has  = reg.has<Position>(e);                          // component existence
reg.remove<Position>(e);                                   // remove component

// Const access
const Registry& cr = reg;
const auto& cp = cr.get<Position>(e);                       // const get
const auto* tp = cr.try_get<Position>(e);                   // const try_get

// View — iterate entities with matching components
for (Entity e : reg.view<Position>()) {                     // single component
    auto& p = reg.get<Position>(e);
}

for (Entity e : reg.view<Position, Velocity>()) {           // multi-component
    auto& p = reg.get<Position>(e);
    auto& v = reg.get<Velocity>(e);
}

// View.each — callback-based iteration
reg.view<Position>().each([&](Entity e, Position& p) {
    p.x += 1.0f;
});

reg.view<Position, Velocity>().each([&](Entity e, Position& p, Velocity& v) {
    p.x += v.vx * dt;
});
```

### Component Concept & Traits

```cpp
#include <exd/ecs/component.hpp>

// Component concept — types must be trivially move-constructible and destructible
static_assert(Component<Position>);    // POD structs satisfy this
static_assert(!Component<int*>);       // pointers are excluded

// Each component type gets a runtime ID (useful for serialization)
uint32_t id = ComponentTraits<Position>::id();
```

### ISystem

```cpp
#include <exd/ecs/system.hpp>

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void update(Registry& registry, double dt) = 0;
    virtual const void* active_modes() const { return nullptr; }  // mode-gating
};
```

---

## exd::core — Foundation Utilities

### Config — Key-Value Store with TOML I/O

```cpp
#include <exd/core/config.hpp>

Config cfg;

cfg.set("window_width", "1920");
cfg.set("fov", 90.0f);                        // template overload via std::to_string
cfg.set("title", "My App");                   // const char* overload
cfg.set_default("vsync", "true");             // only sets if key is missing

auto val  = cfg.get("window_width");          // std::optional<std::string>
int w     = cfg.get_or<int>("window_width", 800);   // typed extraction with default
bool has  = cfg.has("fov");

cfg.load_toml("settings.toml");               // key = "value" format
cfg.save_toml("settings.toml");               // saves all key-value pairs
```

### Logging — Thread-Safe printf-Style Output

```cpp
#include <exd/core/logging.hpp>

exd::core::log_debug("Frame %d, dt=%.4f", frame, dt);
exd::core::log_info("Application started");
exd::core::log_warn("Asset not found: %s", path);
exd::core::log_error("Failed to initialize renderer");

// Levels: Debug, Info, Warning, Error
// Debug/Info/Warning → stdout, Error → stderr
// Thread-safe via internal mutex
```

### Units — Angle Conversion & Physics Helpers

```cpp
#include <exd/core/units.hpp>
using namespace exd::core::units;

double deg = radians_to_degrees(M_PI);         // 180.0
double rad = degrees_to_radians(90.0);         // M_PI / 2

// Lattice Boltzmann force for rectangular duct flow
double f = f_from_u_rectangular_duct(width, height, rho, nu, target_u);
```

### LinearAllocator — Bump Allocator

Per-frame temporary allocations. Header in `include/exd/core/allocator.hpp`.

```cpp
#include <exd/core/allocator.hpp>

LinearAllocator alloc(1024 * 1024);            // 1 MB

void* p1 = alloc.allocate(128);                // 8-byte aligned by default
void* p2 = alloc.allocate(64, 16);             // 16-byte alignment

size_t used = alloc.used();                    // bytes consumed
alloc.reset();                                 // free all (bump pointer back to 0)

// Returns nullptr on overflow — no exceptions
```

### EventBus — Type-Erased Publish/Subscribe

Header in `src/core/event.h` (internal).

```cpp
#include "core/event.h"

EventBus bus;

bus.on("key_press", [](const void* payload) {
    auto* key = static_cast<const char*>(payload);
    // handle key
});

int value = 42;
bus.emit("data_event", &value);                // pass payload pointer

bus.clear();                                   // remove all listeners
```

Global singleton convenience (from `event.cpp`):

```cpp
void event_on(const char* event, EventBus::Callback cb);
void event_emit(const char* event, const void* payload);
```

### Serialization — Binary Blob & JSON Helpers

Header in `include/exd/core/serialization.hpp`.

```cpp
#include <exd/core/serialization.hpp>

// Binary
auto blob = serialize_binary(&my_struct, sizeof(my_struct));
deserialize_binary(blob, &out_struct, sizeof(out_struct));

write_binary_file("data.bin", blob);
auto read = read_binary_file("data.bin");

// JSON
std::string safe = json_escape("say \"hello\"");  // → "say \"hello\""
```

### Plugin Loader — dlopen Wrapper

```cpp
// Available through src/core/plugin.cpp (no public header yet)

void* handle = plugin_load("libmyplugin.so");
void* sym    = plugin_get_symbol(handle, "plugin_init");
plugin_unload(handle);
```

### WindowState — Abstract Input Interface

Header in `include/exd/core/window_state.hpp`. Abstract interface for platform-agnostic input state, moved from `extropian-app` to `extropian-core` to break the dependency chain.

```cpp
#include <exd/core/window_state.hpp>

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
    // ... gamepad state ...
};
```

Both platforms implement this interface:
- **Desktop**: `exd::app::Window` (SDL3-backed, in extropian-app)
- **Web**: Web input handler (in canvas-web)

### Debug Macros

```cpp
#include <exd/core/macros.hpp>

// OpenGL error checking (requires <glad/gl.h> included first)
GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));

// In NDEBUG (release) builds, GL_CALL expands to just the command.
// In debug builds, it checks glGetError() after each call.
```

---

## Building

```bash
# Build (debug)
./build.sh

# Build (release)
./build.sh --release

# Clean
./clean.sh

# Run all tests
./test.sh
```

Or manually:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `EXT_CORE_BUILD_TESTS` | `ON` | Build the test suite (requires FetchContent for doctest) |
| `CMAKE_BUILD_TYPE` | — | `Debug` or `Release` |

### Integration via FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(exd-core
    GIT_REPOSITORY https://github.com/wesleycramblitt/extropian-core.git
    GIT_TAG main
)
FetchContent_MakeAvailable(exd-core)

target_link_libraries(my_app PRIVATE exd::core)
```

---

## Dependencies

None at runtime. C++23 and the C++ standard library only.

Test dependencies are fetched automatically by CMake:
- [doctest](https://github.com/doctest/doctest) v2.4.11 (test framework)

---

## Namespace

```
exd::math    — Vec2-4, Mat3-4, Quat, DualQuat, Bounds3, Raycast, Color
exd::ecs     — Entity, Registry, Component, ISystem, View, CommandBuffer, SystemGraph
exd::core    — Config, logging, units, allocator, event bus, serialization, plugin, WindowState
```

Umbrella header: `#include <exd/ext.hpp>` pulls in all public modules.

---

## License

Business Source License 1.1 — see [LICENSE](LICENSE).  
Converts to Apache 2.0 on 2029-05-26.
