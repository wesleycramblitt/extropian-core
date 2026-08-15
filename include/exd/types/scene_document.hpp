// Scene Document (Doc 3) — THE AUTHORITY — unified spatial tree.
//
// Part of the 4-document pipeline:
//   Doc 1: Semantic Document (what exists)
//   Doc 2: VisualPlan (semantic composition)
//   Doc 3: Scene Document (spaces, nodes, transforms) ← THIS IS THE AUTHORITY
//   Doc 4: Presentation State (runtime focus, annotations)
//
// CRITICAL: This is the unified contract. The AI produces JSON matching these
// structs. There is **no TypeScript mirror** — the JSON contract is C++-only;
// `composer-web` consumes it through WASM.
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

// Pull in PresentationState so SceneDocument can reference it
#include <exd/types/presentation_state.hpp>
// Pull in StyleProfile so SceneDocument can carry density resolution (§5.2)
#include <exd/types/style_profile.hpp>

namespace exd
{

// ═══════════════════════════════════════════════════════════════════
// Diagram Primitives — shapes, ports, visual encoding, container layout
// ═══════════════════════════════════════════════════════════════════

// 2D geometric shape vocabulary (used by NodeType::Shape).
enum class ShapeType
{
    Rect,          // plain rectangle
    RoundedRect,   // rectangle with rounded corners
    Circle,
    Ellipse,
    Diamond,       // rhombus (decision node)
    Hexagon,
    Parallelogram, // skewed box (I/O)
    Triangle,      // directional marker
    Pill,          // stadium / capsule
    Cylinder,      // database / memory / storage
    Stack,         // vertical stack of plates (memory stack)
    Grid,          // N×M array of cells (thread-block grid, tensor)
    Strip,         // horizontal lane of cells (warp / SIMD lane)
    Document,      // note with folded corner
};
// Serialize as PascalCase strings to match the TypeScript mirror.
NLOHMANN_JSON_SERIALIZE_ENUM(ShapeType, {
    {ShapeType::Rect,          "Rect"},
    {ShapeType::RoundedRect,   "RoundedRect"},
    {ShapeType::Circle,        "Circle"},
    {ShapeType::Ellipse,       "Ellipse"},
    {ShapeType::Diamond,       "Diamond"},
    {ShapeType::Hexagon,       "Hexagon"},
    {ShapeType::Parallelogram, "Parallelogram"},
    {ShapeType::Triangle,      "Triangle"},
    {ShapeType::Pill,          "Pill"},
    {ShapeType::Cylinder,      "Cylinder"},
    {ShapeType::Stack,         "Stack"},
    {ShapeType::Grid,          "Grid"},
    {ShapeType::Strip,         "Strip"},
    {ShapeType::Document,      "Document"},
})

// Connection point on a node, for edge routing.
struct Port
{
    std::string id;
    std::string side = "east";   // "north" | "east" | "south" | "west"
    float position = 0.5f;       // 0..1 along the side (0 = left/top)
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Port, id, side, position)

// A named scale mapping a metric domain to a visual range. Shared across
// nodes so "color = complexity" stays consistent and legend-able.
struct ScaleDef
{
    std::string id;
    std::string type = "linear"; // linear | log | sqrt | threshold | quantize | ordinal
    std::string scheme;          // color scheme: viridis | magma | inferno | plasma |
                                 //   blues | diverging | category10 | category20 (empty = none)
    nlohmann::json domain;       // [min,max] or [category,...]
    nlohmann::json range;        // [min,max] (size/opacity) or [color,...] (optional)
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScaleDef, id, type, scheme, domain, range)

// Binds one visual channel to a data field, scaled by a named ScaleDef.
struct ChannelSpec
{
    std::string source;                // data path, e.g. "metrics.code_size"
    std::optional<std::string> scale;  // id of a ScaleDef in SceneDocument.scales
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ChannelSpec, source, scale)

// Per-node visual encoding: metric → visual channel.
struct Encoding
{
    std::optional<ChannelSpec> size;
    std::optional<ChannelSpec> color;
    std::optional<ChannelSpec> opacity;
    std::optional<ChannelSpec> shape;
    std::optional<ChannelSpec> label;
    std::optional<ChannelSpec> edge_width;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Encoding, size, color, opacity, shape, label, edge_width)

// Container layout algorithm — how a space arranges its child nodes.
enum class LayoutAlgorithm
{
    Manual,    // explicit Transform.position
    Grid,      // regular rows/columns
    Layered,   // Sugiyama-style layered DAG (needs relations)
    Tree,      // tidy tree (needs node.children nesting)
    Radial,    // radial tree (needs node.children nesting)
    Force,     // force-directed (needs relations)
    Treemap,   // squarified treemap (size_by)
    Pack,      // circle packing (size_by)
    Swimlane,  // lanes grouped by lane_by
    Timeline,  // time axis by time_by
};
// Serialize as lowercase strings to match the TypeScript mirror.
NLOHMANN_JSON_SERIALIZE_ENUM(LayoutAlgorithm, {
    {LayoutAlgorithm::Manual,   "manual"},
    {LayoutAlgorithm::Grid,     "grid"},
    {LayoutAlgorithm::Layered,  "layered"},
    {LayoutAlgorithm::Tree,     "tree"},
    {LayoutAlgorithm::Radial,   "radial"},
    {LayoutAlgorithm::Force,    "force"},
    {LayoutAlgorithm::Treemap,  "treemap"},
    {LayoutAlgorithm::Pack,     "pack"},
    {LayoutAlgorithm::Swimlane, "swimlane"},
    {LayoutAlgorithm::Timeline, "timeline"},
})

// Container-level layout: how a space arranges its child nodes.
struct DiagramLayout
{
    LayoutAlgorithm algorithm = LayoutAlgorithm::Manual;
    std::optional<ChannelSpec> size_by;   // treemap/pack: channel driving area
    std::optional<ChannelSpec> color_by;  // optional channel driving child color
    std::optional<ChannelSpec> lane_by;   // swimlane: channel grouping nodes into lanes
    std::optional<ChannelSpec> time_by;   // timeline: channel positioning nodes on the time axis
    std::optional<ChannelSpec> start_by;  // timeline (gantt): channel for bar start
    std::optional<ChannelSpec> end_by;    // timeline (gantt): channel for bar end
    nlohmann::json params;                // per-algorithm: gap, cols, rankdir, orientation, node_width, node_height, padding, ...
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DiagramLayout, algorithm, size_by, color_by, lane_by, time_by, start_by, end_by, params)

// ═══════════════════════════════════════════════════════════════════
// Space Types
// ═══════════════════════════════════════════════════════════════════

enum class SpaceType
{
    Screen,
    Panel,
    Cartesian2D,
    Viewport3D,
    World3D,
    Overlay
};
// Serialize as lowercase strings to match the TypeScript mirror.
NLOHMANN_JSON_SERIALIZE_ENUM(SpaceType, {
    {SpaceType::Screen,      "screen"},
    {SpaceType::Panel,       "panel"},
    {SpaceType::Cartesian2D, "cartesian2d"},
    {SpaceType::Viewport3D,  "viewport3d"},
    {SpaceType::World3D,     "world3d"},
    {SpaceType::Overlay,     "overlay"},
})

struct CameraPose
{
    std::array<float, 3> look_at = {0, 0, 5};
    std::array<float, 3> target  = {0, 0, 0};
    std::array<float, 3> up      = {0, 1, 0};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraPose, look_at, target, up)

struct Camera
{
    std::string projection = "perspective";   // "perspective" | "orthographic"
    float fov = 60.0f;
    float near_plane = 0.1f;
    float far_plane = 1000.0f;
    CameraPose pose;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Camera, projection, fov, near_plane, far_plane, pose)

struct GridHint
{
    bool visible = false;
    float size = 10.0f;
    int subdivisions = 10;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GridHint, visible, size, subdivisions)

struct SpaceLayout
{
    std::string x = "0";              // pixel value or percentage string like "60%"
    std::string y = "0";
    std::string width = "100%";
    std::string height = "100%";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpaceLayout, x, y, width, height)

struct Space
{
    std::string id;
    SpaceType type;
    std::optional<std::string> parent;       // parent space id, null for screen
    std::optional<SpaceLayout> layout;       // position within parent
    std::string projection = "orthographic";  // "orthographic" (fixed 2D) | "perspective" (camera 3D)
    std::string background = "#1a1a2e";
    std::optional<Camera> camera;            // viewport3d only
    std::optional<GridHint> grid;            // world3d only
    bool scroll = false;                     // panel only
    std::optional<DiagramLayout> arrangement; // how children are laid out (2D diagrams)
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Space, id, type, parent, layout, projection, background, camera, grid, scroll, arrangement)

// ═══════════════════════════════════════════════════════════════════
// Node Types
// ═══════════════════════════════════════════════════════════════════

enum class NodeType
{
    Panel,
    Text,
    Equation,
    Matrix,
    Plot,
    Vector,
    Curve,
    Mesh,
    Volume,
    Label,
    Graph,
    Code,
    Image,
    Viewport,
    Group,
    Table,
    Form,
    Button,
    Shape,
    Legend
};
// Serialize as PascalCase strings to match the TypeScript mirror.
NLOHMANN_JSON_SERIALIZE_ENUM(NodeType, {
    {NodeType::Panel,    "Panel"},
    {NodeType::Text,     "Text"},
    {NodeType::Equation, "Equation"},
    {NodeType::Matrix,   "Matrix"},
    {NodeType::Plot,     "Plot"},
    {NodeType::Vector,   "Vector"},
    {NodeType::Curve,    "Curve"},
    {NodeType::Mesh,     "Mesh"},
    {NodeType::Volume,   "Volume"},
    {NodeType::Label,    "Label"},
    {NodeType::Graph,    "Graph"},
    {NodeType::Code,     "Code"},
    {NodeType::Image,    "Image"},
    {NodeType::Viewport, "Viewport"},
    {NodeType::Group,    "Group"},
    {NodeType::Table,    "Table"},
    {NodeType::Form,     "Form"},
    {NodeType::Button,   "Button"},
    {NodeType::Shape,    "Shape"},
    {NodeType::Legend,   "Legend"},
})

struct Transform
{
    std::array<float, 3> position = {0, 0, 0};
    std::array<float, 4> rotation = {0, 0, 0, 1};   // quaternion [x,y,z,w]
    std::array<float, 3> scale = {1, 1, 1};
    std::string anchor = "center";
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Transform, position, rotation, scale, anchor)

struct Orient
{
    std::string mode = "fixed";               // "fixed" | "billboard" | "billboard_y"
    std::string face = "camera";              // only used with billboard
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Orient, mode, face)

struct LayoutHint
{
    std::string strategy = "column";          // row | column | grid | absolute | stack | overlay
    float gap = 8.0f;
    float padding = 16.0f;
    std::string alignment = "start";          // start | center | end | stretch
    std::optional<float> min_width;
    std::optional<float> max_width;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LayoutHint, strategy, gap, padding, alignment, min_width, max_width)

struct DataBinding
{
    std::string bind;                         // named data source
    std::optional<std::string> path;          // subpath like "[0][0]"
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DataBinding, bind, path)

struct NodeSemantic
{
    std::string role;
    std::string concept_id;                   // mathematical or domain concept (renamed from 'concept' — C++20 keyword)
    std::string kind;                         // matrix | vector | point | scalar | function | process | constraint
    std::string explanation;
    std::vector<std::string> tags;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NodeSemantic, role, concept_id, kind, explanation, tags)

struct NodeInteraction
{
    bool hover = true;
    bool select = true;
    bool drag = false;
    bool focus = true;
    bool inspect = true;
    bool edit = false;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NodeInteraction, hover, select, drag, focus, inspect, edit)

struct NodeStyle
{
    // Canonical emphasis vocabulary, shared with StyleOverride (presentation_state.hpp).
    std::string emphasis = "default";         // subtle | default | primary | prominent
    float opacity = 1.0f;
    int depth = 0;
    bool visible = true;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(NodeStyle, emphasis, opacity, depth, visible)

struct SceneNode
{
    std::string id;
    NodeType type;
    std::string space;                        // which space this node lives in
    std::optional<Transform> transform;
    std::optional<Orient> orient;
    std::optional<LayoutHint> layout;
    nlohmann::json geometry;                  // type-specific: see node type table
    nlohmann::json content;                   // type-specific: text, value, series, etc.
    std::optional<DataBinding> data;
    std::optional<NodeSemantic> semantic;
    std::optional<Encoding> encode;           // visual encoding: metric → channel
    std::vector<Port> ports;                  // connection points for edge routing
    std::optional<DiagramLayout> arrangement; // container: lay out children inside this node's bounds
    NodeInteraction interaction;
    NodeStyle style;
    std::vector<SceneNode> children;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SceneNode, id, type, space, transform, orient, layout, geometry, content, data, semantic, encode, ports, arrangement, interaction, style, children)

// ═══════════════════════════════════════════════════════════════════
// Relations
// ═══════════════════════════════════════════════════════════════════

struct RelationStyle
{
    std::string type = "arrow";               // arrow | line | tube | bezier | elbow
    std::string color = "#4a9eff";
    float width = 2.0f;
    bool dash = false;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RelationStyle, type, color, width, dash)

struct RelationLabel
{
    std::string text;
    std::string position = "middle";          // start | middle | end
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RelationLabel, text, position)

struct RelationSemantic
{
    std::string kind;                         // transform | dependency | data_flow |
                                              // hierarchy | comparison | constraint
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RelationSemantic, kind)

struct Relation
{
    std::string id;
    std::string source;                       // node id
    std::string target;                       // node id
    std::optional<std::string> source_port;
    std::optional<std::string> target_port;
    RelationStyle style;
    std::optional<RelationLabel> label;
    std::optional<RelationSemantic> semantic;
    int bundle = 1;                       // number of logical edges this represents (e.g. a warp's 32 lanes)
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Relation, id, source, target, source_port, target_port, style, label, semantic, bundle)

// ═══════════════════════════════════════════════════════════════════
// Top-Level SceneDocument
// ═══════════════════════════════════════════════════════════════════

struct SceneDocument
{
    int version = 1;
    std::string topic;
    std::vector<Space> spaces;
    std::vector<SceneNode> nodes;             // root nodes (children nested inside)
    std::vector<Relation> relations;
    std::optional<PresentationState> presentation;   // from presentation_state.hpp
    nlohmann::json state;                     // reactive data state
    nlohmann::json data_sources;              // named data sources for bindings
    std::vector<ScaleDef> scales;             // shared visual scales (metric → channel)
    std::optional<StyleProfile> style_profile; // density → typography/spacing metrics (§5.2, additive)
    std::optional<std::string> composition;    // informational macro-strategy id (additive)
};
// Macro-based serialization doesn't handle std::optional fields correctly,
// so we need explicit to_json/from_json for SceneDocument.
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE is used for all types above that don't
// contain std::optional fields.

inline void to_json(nlohmann::json& j, const SceneDocument& doc)
{
    j = nlohmann::json{
        {"version",       doc.version},
        {"topic",         doc.topic},
        {"spaces",        doc.spaces},
        {"nodes",         doc.nodes},
        {"relations",     doc.relations},
        {"state",         doc.state},
        {"data_sources",  doc.data_sources},
        {"scales",        doc.scales},
    };
    if (doc.presentation.has_value())
        j["presentation"] = doc.presentation.value();
    else
        j["presentation"] = nullptr;
    if (doc.style_profile.has_value())
        j["style_profile"] = doc.style_profile.value();
    else
        j["style_profile"] = nullptr;
    if (doc.composition.has_value())
        j["composition"] = doc.composition.value();
    else
        j["composition"] = nullptr;
}

inline void from_json(const nlohmann::json& j, SceneDocument& doc)
{
    j.at("version").get_to(doc.version);
    j.at("topic").get_to(doc.topic);
    j.at("spaces").get_to(doc.spaces);
    j.at("nodes").get_to(doc.nodes);
    j.at("relations").get_to(doc.relations);
    j.at("state").get_to(doc.state);
    j.at("data_sources").get_to(doc.data_sources);
    if (j.contains("scales"))
        j.at("scales").get_to(doc.scales);
    if (j.contains("presentation") && !j.at("presentation").is_null())
        doc.presentation = j.at("presentation").get<PresentationState>();
    if (j.contains("style_profile") && !j.at("style_profile").is_null())
        doc.style_profile = j.at("style_profile").get<StyleProfile>();
    if (j.contains("composition") && !j.at("composition").is_null())
        doc.composition = j.at("composition").get<std::string>();
}

} // namespace exd
