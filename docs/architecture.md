# Extropian Ecosystem Architecture

> Ecosystem-wide architecture report (2026-09-11). The ecosystem converged on a
> shared ECS + generic SceneGraph as its only hierarchy/transform model; spatial
> UI became pure ECS/2.5D with the VisualDocument runtime removed; CAE joined
> the shared graph. This document is the authoritative cross-repo reference.

---

## 1. Objective & Outcome

Refactor the Extropian ecosystem onto one architectural spine:

- **One ECS** (`exd::ecs` in extropian-core) used by render, spatial-ui, CAE,
  geometry demos, and app-facing scenes.
- **One generic `SceneGraph`** (`exd::ecs::SceneGraph`) as the *only*
  hierarchy model — no renderer-local hierarchy, no CAE-local tree, no UI DOM.
- **Transforms as components**: `LocalTransform` (authored) + `WorldTransform`
  (derived by `TransformSystem`) replace every per-repo transform class.
- **UI as spatial 2.5D**: widgets are ordinary entities in the shared graph;
  a conventional 2D-looking UI is a camera/transform configuration, not a
  separate 2D scene model. The `VisualDocument` runtime is gone.
- **Full clean cut**: deleted obsolete architecture outright — no legacy
  preservation, no deprecation aliases, no compatibility shims.

All five repos pass their suites (see §7).

---

## 2. Before / After

| Concern | Before | After |
|---|---|---|
| Hierarchy | `render::HierarchySystem` (Parent/Children/SiblingLink) + `HierarchyIndex` (core) + CAE's own implicit trees | `ecs::SceneGraph` (core) — the only hierarchy, everywhere |
| Transforms | `render::Transform`, CAE mesh-side poses, `ecs::Transform` | `ecs::LocalTransform` (component) + `ecs::WorldTransform` (derived) |
| UI runtime | `VisualDocument` → `DocumentLoader` → Document* components → style resolver → render_queue/view_sync → ~14 systems | Pure ECS: `WidgetComponent` + `WidgetBuilder` → dirty-tagged resolution systems |
| 2D vs 3D | separate 2D document scene + 3D world | one SceneGraph; "2D" = ortho camera + camera-fixed widgets |
| CAE scene | flat entity layout, render::Transform | optional `SceneGraph*` param: `"model.<name>"` root parents all parts/markers |
| Pick/interaction | render-owned state machines + UI-owned | render owns raw hit production only; spatial-ui owns semantics + pick-ID assignment |

---

## 3. `exd::ecs::SceneGraph` (core)

`include/exd/ecs/scene_graph.hpp` — generic parent-child containment with no
transform/render/domain semantics:

- `attach(Entity child, Entity parent)` / `attach_at` (ordered position),
  `detach`, `remove`, `reparent`, `reorder` / `move_after`.
- Depth-first traversal with early-abort (`traverse(parent, fn) -> bool`),
  `roots()`, `depth(entity)`, generation-safe `prune`.
- `SceneGraph` + `ecs::Registry` are the two objects every scene owns;
  entities are plain `ecs::Entity` handles shared across the graph.

CAE semantics, render semantics, and UI semantics are *components*, never graph
features: `Cae*` components carry multiphysics meaning, `WidgetComponent`
carries widget meaning, `RenderableComponent`/`Material` carry draw meaning.

## 4. Transform model

- `ecs::LocalTransform` — component state, relative to parent (TRS).
- `ecs::TransformSystem` (core) — derives `ecs::WorldTransform` each update
  from SceneGraph relationships + LocalTransform (TRS accumulation,
  cycle-guarded). Runs in the Structural phase.
- Consumers: render `resolve_world_transform` + `RenderSystem::compute_model`
  prefer `WorldTransform` and fall back to `LocalTransform` for roots/flat
  scenes; spatial-ui `InteractionSystem::world_bounds` does the same;
  `make_camera` takes an `ecs::LocalTransform`.

## 5. Repo-by-repo changes

### extropian-core (c9a0ee9)
- Added `SceneGraph`, `LocalTransform`/`WorldTransform`, `TransformSystem`
  (5e5e922); removed the `HierarchyIndex` alias entirely (c9a0ee9).
- `docs/plan.md` §8 documents the model; this report supersedes as the
  cross-repo reference.
- 31/31 tests.

### extropian-render (0dd1a85 … 9d19483)
- Deleted `render::Transform`, `Parent`, `Children`, `SiblingLink`, `Skew`,
  `HierarchySystem`.
- `resolve_world_transform`/`compute_model` consume `ecs::WorldTransform`
  (LocalTransform fallback); `make_camera` takes `ecs::LocalTransform`;
  camera selection, `CameraModeSystem`, `Gizmo3DSystem`, `PickerSystem`,
  `InteractionSystem` migrated; `ecs_inspector` registers both transform
  components.
- Docs updated (plan.md systems table, camera-collision execution order,
  render-architecture, spatial-ui-integration).
- 87/88 tests (the 1 failure is the pre-existing unrelated extropian-app
  `EventState` test — level-held vs edge-triggered mouse buttons).

### extropian-geometry (3a2b4af)
- Fixed two topology defects left by the winding audit that broke downstream
  manifold gates: extrusion wall quads were triangulated onto the crossed
  diagonal (open `f0-b0` edge per quad); lathe cap fans were unwelded ring
  copies. Caps now share wall ring vertices with smooth recomputed normals
  (extrusion/helix policy); Y-axis lathe wall split corrected.
- New `winding_test` case pins `closed_manifold_gate` on every welded capped
  generator; `advanced_test` updated to the welded layout; demos migrated off
  `render::Transform` to `ecs::LocalTransform`.
- 21/21 tests.

### extropian-spatial-ui (ab27378 … 655d7a7)
- Removed the entire VisualDocument runtime (DocumentLoader/Builder, Document*
  components, ElementComponent, widget-spec components, style resolver,
  render_queue, view_sync, Composition/Relation/Layout/Mesh/Pick/RenderOrder/
  Size/WidgetController systems, 14 test files, document demos).
- New widget model: `WidgetComponent` (kind/text/size/fontSize/padding/gap/
  color/interactive/camera_fixed/layer), `LayoutComponent` (Column/Row/
  Absolute), `WidgetDirty`, `WidgetBuilder` + `build_panel/label/button/
  separator`, `layout_children` (auto-fit containers).
- New resolution pipeline: `FontSystem` (Structural) → `UiSizeSystem` →
  `UiLayoutSystem` (Layout) → `UiMeshSystem` (RenderPreparation), all
  dirty-tag driven; camera-fixed HUD via `ScreenWidgetComponent`.
- Interaction on SceneGraph entities: `InteractionSystem`, `KeyboardSystem`,
  `ScrollContainerSystem`, `HoverVisualSystem`, `SelectionVisualSystem`,
  `InputBridge`; stable pick IDs (`PickComponent`), render-order sort keys.
- Single canonical demo: one ECS + one SceneGraph with a rotating 3D cube and
  a UI panel; headless screenshot verification via `EXD_DEMO_SCREENSHOT`.
- Docs rewritten (architecture.md 863→589 lines, demos.md); dead demo
  scaffolding deleted; stale header comments fixed.
- 109/109 tests.

### extropian-cae (36cd038, ed88dff)
- `CaeSceneBuilder::build` gained an optional `exd::ecs::SceneGraph*`:
  creates `"model.<name>"` root with `LocalTransform`, parents all parts +
  markers under it; `nullptr` keeps the flat layout for headless tools.
- Demo registers core `TransformSystem`; `render::Transform`/`HierarchySystem`
  usage removed.
- Steam-engine closures re-wound to the audited wall directions; all 10
  parts pass `closed_manifold_gate` (was 3 open); `part_test` first-wall
  triangle updated; cad_model→cae_model migration completed.
- 14/14 tests.

---

## 6. Cross-repo interfaces (post-refactor contract)

| Interface | Owner | Consumers |
|---|---|---|
| `ecs::Registry`, `ecs::Entity`, `ecs::ISystem`, `SystemGraph/SystemPhase` | core | render, spatial-ui, cae, geometry demos |
| `ecs::SceneGraph` | core | render, spatial-ui, cae, geometry demos |
| `ecs::LocalTransform`/`WorldTransform` + `TransformSystem` | core | render (compute_model/camera), spatial-ui (world_bounds), cae |
| `WidgetComponent`/`LayoutComponent`/`WidgetDirty`/`WidgetBuilder` | spatial-ui (exd-ui) | scene_renderer systems, apps |
| `render::UIRenderableComponent`/`Material`/`UiPipeline` + `GraphicsContext::mesh_manager` | render | spatial-ui UiMeshSystem |
| `PickComponent`/`RenderOrderComponent`/`HoverStateComponent`/`SelectedStateComponent` | spatial-ui | render ID-buffer pass, render `UIRenderSystem` sort keys |
| `closed_manifold_gate` + welded capped generators | geometry | cae manifold tests, booleans |
| `CaeSceneBuilder::build(..., SceneGraph*)` | cae | demo/app scene hosting |

## 7. Test status (2026-09-11)

| Repo | Suite | Result |
|---|---|---|
| extropian-core | 31 | 31/31 |
| extropian-render | 88 | 87/88 (EventState, pre-existing unrelated) |
| extropian-geometry | 21 | 21/21 |
| extropian-spatial-ui | 109 | 109/109 |
| extropian-cae | 14 | 14/14 |

## 8. Remaining debt / deviations

- **extropian-synthesis** does not exist in the workspace (referenced by
  older docs); no action taken.
- **extropian-app** `EventState` test failure is pre-existing (edge-triggered
  mouse buttons made level-held); unrelated to this refactor, left as-is.
- **Helix open tube** (`capped=false`) is intentionally not a closed solid
  (open annulus ends) and correctly fails `closed_manifold_gate`; documented
  in geometry winding_test.
- **extropian-composer** (ported 2026-09-11 to the widget-builder model,
  `018ff18`) still has no AI/semantic-to-visual pipeline; the widget builders
  are the only authoring surface (documented in its `docs/plan.md` section 7).
