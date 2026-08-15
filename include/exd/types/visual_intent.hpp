// Canonical VisualIntent types — shared by conductor and compiler.
//
// ⚠️ LEGACY / DEPRECATED (2026-08). Superseded by `VisualPlan` (Doc 2, see
// visual_plan.hpp). Kept for backward compatibility only; new consumers should
// emit/parse `VisualPlan` and let `VisualPlanCompiler` resolve it into a
// `SceneDocument`.
//
// This was the single source of truth for the VisualIntent document model.
// Both the AI conductor and the deterministic compiler operated on these types.
//
// Pipeline:  Conductor (AI) → VisualIntent → Compiler → ECS → Render
//
// Namespace: exd::types  (neutral territory, not tied to conductor or compiler)
#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace exd::types
{

// ── Block kind ──
//
// What kind of visual block this is. Used for coarse widget dispatch.
// The string `type` field on Block provides finer-grained widget selection
// (e.g. "Chart" vs "Graph" when both are BlockKind::Visualization).

enum class BlockKind
{
    Heading,
    Text,
    Math,
    Table,
    Diagram,
    Chart,
    Visualization,
    Callout,
    Code,
    List,
    Image,
    Spacer,
    Divider,
    Container,
    Custom
};

// ── Relationship kind ──

enum class RelationshipKind
{
    Connects,       // generic connection
    Causes,         // A causes B
    FlowsTo,        // A flows into B
    DependsOn,      // A depends on B
    Controls,       // A controls B
    TransformsInto, // A becomes B
    References,     // A references B
    Annotates,      // A annotates/labels/explains B (tooltip, label, callout)
    Contains,       // A contains B (hierarchical; prefer Entity.children for primary hierarchy)
    Custom
};

// ── Connector style ──
//
/// Visual style for a relationship connector edge.

enum class ConnectorStyle
{
    Line,        // straight line
    Arrow,       // line with arrowhead
    Elbow,       // orthogonal (right-angle) routing
    Bezier,      // curved bezier edge
    Tube,        // 3D tube/cable
    Dashed,      // dashed line
    Dotted,      // dotted line
    Band,        // thin parallel lines representing N relationships
    Bundle,      // merged curve for dense edge groups
};

// ── Block ──
//
/// A single visual block in the document. This is what the AI conductor
/// emits. Blocks form a hierarchy via `children` (block IDs).
///
/// The renderer resolves layout, style, and widget generation from
/// the block kind, type string, and content. The `properties` map carries
/// semantic data (e.g. chart values, graph edge weights) that the
/// extropian-widgets builder consumes during WidgetAssembly.

struct Block
{
    std::string id;
    std::string type;   // "Panel", "Chart", "Graph", "Shape", "Text", etc.
    std::string name;   // Human-readable label
    BlockKind   kind = BlockKind::Container;

    /// Semantic properties consumed by widget builders and data binding.
    /// E.g. {"value": "42", "color": "#FF0000", "label": "Revenue"}
    std::unordered_map<std::string, std::string> properties;

    /// Block IDs of child blocks (hierarchy).
    std::vector<std::string> children;
};

// ── Relationship ──
//
/// An edge between two blocks. Used for graph/diagram views.
/// The renderer may create connector entities for these.

struct Relationship
{
    std::string       id;
    std::string       source;  // block ID
    std::string       target;  // block ID
    RelationshipKind  kind = RelationshipKind::Connects;

    /// Edge attributes for visual encoding (thickness, animation, filtering).
    std::optional<float> weight;
    std::optional<float> confidence;

    /// Visual style override for the connector edge.
    ConnectorStyle connector_style = ConnectorStyle::Arrow;
};

// ── ViewSpec ──
//
/// Declares how the VisualIntent should be presented:
/// layout strategy, visual encoding mappings, interaction affordances,
/// and theming. This is the "view" layer — separate from the semantic
/// block data so themes and layouts can be swapped without reprocessing
/// the blocks.

struct ViewSpec
{
    // ── Encoding: semantic property → visual channel ──

    struct Encoding
    {
        std::string color_by;      // property name to map to color hue
        std::string size_by;       // property name to map to entity size
        std::string opacity_by;    // property name to map to opacity
        std::string edge_width_by; // property name for connector thickness
    };

    // ── Interaction: user affordances ──

    struct Interaction
    {
        bool selectable = true;
        bool hoverable  = true;
        bool draggable  = true;
        bool expandable = true;
    };

    std::string layout;              // "force-directed", "tree", "column", etc.
    Encoding    encoding;            // visual encoding mappings
    Interaction interaction;         // interaction affordances
    std::string theme = "dark";      // theme name (resolved by ThemeRegistry)
};

// ── Incremental compilation hint ──
//
/// Tells the renderer whether to do a full rebuild or an incremental
/// patch. Full rebuild destroys all previous entities and creates new ones.
/// Patch updates only changed blocks (content-hash-based).

enum class IncrementalHint
{
    Full,           // destroy all previous entities, rebuild from scratch
    Incremental,    // content-hash-based partial update on changed blocks
    Force,          // full rebuild even if incremental was requested
};

// ── VisualIntent ──
//
/// The input document from the AI conductor.
/// This is a complete description of what should be on screen.
///
/// For full rebuilds: include all blocks with ViewSpec.
/// For incremental updates: include only changed blocks and relationships.

struct VisualIntent
{
    std::string version  = "1.0";

    std::vector<Block>         blocks;
    std::vector<Relationship>  relationships;
    ViewSpec                   view;
};

} // namespace exd::types
