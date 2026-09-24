# Type Contracts

C++ structs in `include/exd/types/` are the single JSON-contract authority. There is no TypeScript mirror (deprecated with `extropian-web-ui`); `composer-web` consumes the JSON via WASM (`docs/plan.md` §§5–5.2,7; `README.md`).

- `SemanticDocument` (`semantic_document.hpp`) — Doc 1: what concepts exist (semantic input).
- `VisualDocument` (`visual_document.hpp`) — authored declarative visual document: canvas, sections, nodes, relations, data sources, initial state. v1 is deliberately orthographic 2D authored input: shape, geometry, size constraints, layout intent only. Excludes 3D/world spaces, cameras, perspective, resolved x/y/z transforms; resolved scene data stays implementation-owned.
- `VisualDocumentPatch` (`visual_document_patch.hpp`: `VisualDocumentPatchOp`, `VisualDocumentPatch`) — structural mutations for `VisualDocument`.
- `StyleProfile` (`style_profile.hpp`) — deterministic typography/spacing metrics (`density`, `base_font`, `caption_font`, `panel_gap`, `section_gap`, `padding`, `annotation_gap`, `border_width`); presentation code resolves tokens against the active profile, never raw RGB in the document.
- `PresentationState` (`presentation_state.hpp`: `StyleOverride`, `Annotation`, `AnimationClip`, `PatchOp`) — runtime UI state, implementation-owned.
- Shared emphasis vocabulary: `"subtle | default | primary | prominent"` for authored styles and `StyleOverride::emphasis`; runtime UI overrides default to `"subtle"` (dim).

Ecosystem note: the `VisualDocument` *runtime* (DocumentLoader, style resolver, render_queue/view_sync) was removed from extropian-spatial-ui in the 2026-09-11 refactor — UI is now pure ECS widgets in the shared `SceneGraph`. The authored `VisualDocument` input contract in core remains (`docs/architecture.md` §§2,5).

Sources: `docs/plan.md` §§5–5.2,7–8; `docs/architecture.md` §§2,5; `include/exd/types/{semantic_document,visual_document,visual_document_patch,style_profile,presentation_state}.hpp`.
