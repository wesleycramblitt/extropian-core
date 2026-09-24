# Knowledge Map — extropian-core

Foundation library: shared ECS, math, types, infrastructure. No GPU, audio, or platform dependencies.

- [Core ownership](core-ownership.md) — purpose, owns / does-not-own boundary, namespaces, `WindowState` contract.
- [ECS, SceneGraph, transforms](ecs-scene-graph-transform.md) — the only hierarchy/transform model; `Registry` + `SceneGraph` + `Local/WorldTransform` + `TransformSystem`.
- [Type contracts](types-contracts.md) — C++-only JSON schema authority: `SemanticDocument`, authored `VisualDocument` + patch, `StyleProfile`, runtime `PresentationState`.
- [Build and integration](build-integration.md) — C++23, dependency versions, build/test entry points, FetchContent use.

Sources: `docs/plan.md`, `docs/architecture.md` (2026-09-11 ecosystem report, core-only extract), `README.md`, `CMakeLists.txt`, `include/exd/`.
`docs/` retained on disk; removal pending user decision (see Completion report).
