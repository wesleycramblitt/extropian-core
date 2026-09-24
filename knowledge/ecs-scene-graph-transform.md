# ECS, SceneGraph, Transforms

Ecosystem decision (2026-09-11): one shared ECS plus one generic `SceneGraph` as the only hierarchy/transform model. No renderer-local hierarchy, no CAE-local tree, no UI DOM. Full cut — no legacy aliases or shims (`docs/architecture.md` §§1–2; commits `5e5e922`, `c9a0ee9` removed `HierarchyIndex`).

## Registry model

Sparse-set, archetype-free ECS (`include/exd/ecs/registry.hpp`, `entity.hpp`, `view.hpp`, `component.hpp`, `command_buffer.hpp`, `system_graph.hpp`, `system.hpp`; impl `src/ecs/registry.cpp`):

- `Entity` is `{id, gen, name}`; equality on `id+gen`; generation-safe reuse.
- Per-type sparse-set pools; `View<Cs...>` iterates the smallest pool and filters.
- `CommandBuffer` defers mutations during iteration; `SystemGraph` orders `ISystem::update(Registry&, double)`.

## SceneGraph — the only hierarchy

Pure containment over entities; knows nothing about transforms, rendering, UI, layout, cameras, geometry, or physics (`include/exd/ecs/scene_graph.hpp` is authority):

- API (note argument order — source truth; `docs/architecture.md` §3 inverts it): `attach(parent, child)`, `attach_at(parent, child, index)`, `detach(child)`, `remove(entity)`, `reparent`, `reorder`/`move_after`, `traverse(parent, fn)->bool` (DFS, early-abort), `roots()`, `depth(entity)`, generation-safe `prune()`.
- Self-parenting and cycles rejected (`false`).
- `remove` orphans children to roots; does not own entities — orthogonal to `Registry`.
- Equally usable for spatial trees, CAE part trees, UI widget trees; domain meaning lives in components, never in the graph.

## Transforms — component state + derived state

- `LocalTransform` (`include/exd/ecs/transform.hpp`): authored TRS relative to parent. No hierarchy semantics alone.
- `WorldTransform`: derived accumulated local→world TRS. Consumers must prefer it, falling back to `LocalTransform` only for roots/flat scenes.
- `TransformSystem` (`include/exd/ecs/transform_system.hpp`, `src/ecs/transform_system.cpp`): each update derives `WorldTransform` from SceneGraph parent chain + `LocalTransform` (TRS accumulation, cycle-guarded). Parents without `LocalTransform` contribute identity; entities without it are untouched. Also offers static `derive(registry, scene, entity)`.

## Core-owned cross-repo contract

Core owns and downstream consumes (`docs/architecture.md` §6, core rows only):

- `ecs::Registry`, `ecs::Entity`, `ecs::ISystem`, `SystemGraph`/`SystemPhase` → render, spatial-ui, cae, geometry demos.
- `ecs::SceneGraph` → render, spatial-ui, cae, geometry demos.
- `ecs::LocalTransform`/`WorldTransform` + `TransformSystem` → render (`compute_model`/camera), spatial-ui (`world_bounds`), cae.

Sources: `docs/plan.md` §8 (second); `docs/architecture.md` §§2–4,6; `include/exd/ecs/scene_graph.hpp`, `transform.hpp`, `transform_system.hpp`.
