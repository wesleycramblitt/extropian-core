# extropian-core

**Foundation library for all Extropian projects.**

Zero dependencies beyond the C++ standard library. Provides the bedrock types and data structures every Extropian library and application builds upon.

## Contents

| Module | What It Provides |
|--------|-----------------|
| `exd::math` | Vec2, Vec3, Vec4, Mat3, Mat4, Quat, DualQuat, transforms |
| `exd::ecs` | Entity, Registry, Component concept, ISystem, sparse-set views |
| `exd::geom` | Shared geometry types: Vertex, MeshData, PrimitiveTopology, Bounds, Transform, VertexStream, IndexData, vertex/index enums |
| `exd::core` | Allocators, serialization, plugin loader, event bus, config parser, logging, SI units |

## exd::geom — Shared Geometry Types

This module provides the canonical CPU-side geometry types used by the entire ecosystem. Both Canvas and Renderer consume these types, eliminating duplication.

### Key Types

```cpp
// Vertex — interleaved vertex layout
struct Vertex {
    Vec3f position;
    Vec3f normal;
    Vec3f uv;
    Quat  tangent;   // identity by default
    Quat  color;     // packed RGBA as Quat
};

// PrimitiveTopology — draw topology
enum class PrimitiveTopology {
    Points, Lines, LineStrip, Triangles, TriangleStrip
};

// MeshData — CPU-side mesh (ready for GPU upload)
struct MeshData {
    std::vector<Vertex>     vertices;
    std::vector<uint32_t>   indices;
    PrimitiveTopology       topology;
};

// Bounds — axis-aligned bounding box
struct Bounds {
    Vec3f min;
    Vec3f max;
};

// Transform — translation, rotation, scale
struct Transform {
    Vec3f translation{0, 0, 0};
    Quat  rotation = Quat::identity();
    Vec3f scale{1, 1, 1};
};

// Streaming / SoA layout types (for future use)
struct VertexStream { VertexSemantic semantic; ComponentType type; ... };
struct IndexData    { IndexType type; std::vector<std::byte> data; };
```

## Building

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

## Dependencies

None. C++23 and the standard library only.

## Namespace

All types are in `exd::` with sub-namespaces (`exd::math`, `exd::ecs`, `exd::geom`, `exd::core`).

## License

Business Source License 1.1 — see [LICENSE](LICENSE).
Converts to Apache 2.0 on 2029-05-26.
