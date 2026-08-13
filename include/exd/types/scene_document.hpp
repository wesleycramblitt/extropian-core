// Scene Document (Doc 3) — THE AUTHORITY — unified spatial tree.
//
// Part of the 4-document pipeline:
//   Doc 1: Semantic Document (what exists)
//   Doc 2: Visual Intent Document (how to communicate)
//   Doc 3: Scene Document (spaces, nodes, transforms) ← THIS IS THE AUTHORITY
//   Doc 4: Presentation State (runtime focus, annotations)
//
// CRITICAL: This is the unified contract. Both C++ SceneCompiler and
// TypeScript extropian-web-ui consume this format. The AI produces JSON
// matching these structs. If you change these types, the TypeScript mirror
// (extropian-web-ui/src/types.ts) must be updated to stay aligned. The
// mirror is hand-kept and aligned by review — there is no shared test
// fixture harness between the two backends.
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

// Pull in PresentationState so SceneDocument can reference it
#include <exd/types/presentation_state.hpp>

namespace exd
{

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
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Space, id, type, parent, layout, projection, background, camera, grid, scroll)

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
    Button
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
    NodeInteraction interaction;
    NodeStyle style;
    std::vector<SceneNode> children;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SceneNode, id, type, space, transform, orient, layout, geometry, content, data, semantic, interaction, style, children)

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
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Relation, id, source, target, source_port, target_port, style, label, semantic)

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
    };
    if (doc.presentation.has_value())
        j["presentation"] = doc.presentation.value();
    else
        j["presentation"] = nullptr;
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
    if (j.contains("presentation") && !j.at("presentation").is_null())
        doc.presentation = j.at("presentation").get<PresentationState>();
}

} // namespace exd
