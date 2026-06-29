# extropian-core

**Foundation library for all Extropian projects.**

Zero dependencies beyond the C++ standard library. Provides the bedrock types and data structures every Extropian library and application builds upon.

## Contents

| Module | What It Provides |
|--------|-----------------|
| `ext::math` | Vec3, Vec4, Mat3, Mat4, Quat, dual quaternion, transforms |
| `ext::ecs` | Entity, Registry, ISystem, Component<T>, Views, resource cache |
| `ext::geom` | Primitives (sphere, box, capsule, etc.), CSG tree, spatial queries (octree, BVH), SDFs |
| `ext::core` | Allocators, serialization, plugin loader, event bus, config parser, logging, SI units |

## Building

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

## Dependencies

None. C++23 and the standard library only.

## Namespace

All types are in `ext::` with sub-namespaces (`ext::math`, `ext::ecs`, `ext::geom`, `ext::core`).

## License

Business Source License 1.1 — see [LICENSE](LICENSE).
Converts to Apache 2.0 on 2029-05-26.
