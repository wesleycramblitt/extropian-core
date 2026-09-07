// AI-authored declarative 2D visual document.
//
// This contract describes intent and constraints for a renderer. It deliberately
// does not contain resolved transforms, runtime selection/hover state, or 3D
// camera/world data. v1 is intentionally an orthographic 2D contract.
#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <exd/core/json_optional.hpp>

namespace exd
{

struct VisualMetadata
{
    std::string title;
    std::optional<std::string> description;
    std::optional<std::string> author;
    std::vector<std::string> tags;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualMetadata, title, description, author, tags)

/// Canvas grid resolution. rows == 0 derives rows from the canvas aspect
/// ratio (rows = round(cols * height / width)) so cells stay proportional.
struct VisualGrid
{
    int cols = 12;
    int rows = 0;                             // 0 = derive from aspect ratio
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualGrid, cols, rows)

// width/height/aspect_ratio are authored FALLBACKS used only when no live
// window is available (headless/tests); when a window exists the document
// fills the visual viewport (1 unit = 1 window px). background/grid remain
// authored.
struct VisualCanvas
{
    std::optional<float> width;
    std::optional<float> height;
    std::string unit = "px";                 // px | dp | rem
    std::optional<std::string> aspect_ratio; // e.g. "16:9"
    std::optional<std::string> background;
    std::optional<VisualGrid> grid;           // optional grid resolution override
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualCanvas, width, height, unit, aspect_ratio, background, grid)

// A named constraint is resolved by the renderer; it is not a coordinate.
struct LayoutConstraint
{
    std::string kind;                         // before | after | above | below | align | span
    std::optional<std::string> target;        // node or section id
    nlohmann::json value;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LayoutConstraint, kind, target, value)

/// A cell region on the container's grid: (col, row) origin plus span.
/// Coordinates are relative to the parent container's cell region and use
/// the canvas grid step (one shared grid across the document).
struct VisualGridRect
{
    int col = 0;
    int row = 0;
    int cols = 1;
    int rows = 1;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualGridRect, col, row, cols, rows)

/// Anchored floating placement within the parent container's region.
/// edge: north | east | south | west | north_east | north_west |
///       south_east | south_west | center
/// Offsets are distances inward from the anchored edge; each is a number
/// (logical px), a percentage ("12%"), or a rem value ("0.5rem", scaled by
/// the density profile's base font). % offsets are density/viewport-stable.
struct VisualFloatPlacement
{
    std::string edge = "north_east";
    std::optional<std::string> offset_x;
    std::optional<std::string> offset_y;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualFloatPlacement, edge, offset_x, offset_y)

/// Absolute placement within the positioning context: the nearest ancestor
/// whose layout preset is "overlay", else the document root. Coordinates
/// are distances from the content-box top-left corner of the context; each
/// is a number (logical px), a percentage ("12%"), or a rem value
/// ("0.5rem", scaled by the density profile's base font). An absent axis
/// centers the entity along that axis within the context.
struct VisualAbsolutePlacement
{
    std::optional<std::string> x;
    std::optional<std::string> y;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualAbsolutePlacement, x, y)

/// Explicit placement of an entity on its parent container's grid.
/// Presence of `grid` positions the entity on the parent's grid cells
/// (containers fill their cell region; leaves sit centered in it).
/// Presence of `float` anchors the entity to an edge/corner of the parent
/// region (labels, badges, annotations in odd places).
/// Presence of `absolute` positions the entity from the content-box top-left
/// of the positioning context (nearest "overlay" ancestor, else the document
/// root) with CSS position:absolute semantics; an absent axis centers it.
/// Absent: the entity is arranged by the parent's layout preset.
struct VisualPlacement
{
    std::optional<VisualGridRect> grid;
    std::optional<VisualFloatPlacement> float_;
    std::optional<VisualAbsolutePlacement> absolute;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualPlacement, grid, float_, absolute)

/// width/height/min_width/max_width/min_height accept one of:
///   "auto"    default — container: fill available width / content height;
///             leaf: intrinsic size
///   "fill"    explicit stretch to the parent content-box on that axis
///   "<n>"     n logical px          ("<n>%" → % of parent content-box;
///   "<n>rem"  n × density base font)
/// min_*/max_* clamp the resolved size after measurement/distribution.
/// Strings are resolved by the layout consumer (see exd::parse_size_spec).
struct VisualLayout
{
    std::string preset = "stack";            // stack | row | grid | split | overlay | flow
    std::optional<float> gap;
    std::optional<float> padding;
    std::optional<std::string> width;
    std::optional<std::string> height;
    std::optional<std::string> min_width;
    std::optional<std::string> max_width;
    std::optional<std::string> min_height;
    std::optional<std::string> max_height;
    std::optional<std::string> align;
    std::optional<std::string> justify;
    std::vector<LayoutConstraint> constraints;
    nlohmann::json params;                      // authored preset parameters
    std::optional<VisualPlacement> placement;   // explicit placement override
    // Share of leftover space along the parent layout's main axis
    // (flex-grow semantics); absent/0 = no share. Used by stack/row parents
    // after fixed/% children are placed.
    std::optional<float> weight;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualLayout, preset, gap, padding, width, height,
    min_width, max_width, min_height, max_height, align, justify, constraints, params, placement, weight)

enum class VisualNodeKind
{
    Text,
    Panel,
    Shape,
    Chart,
    Plot,
    Graph,
    Legend,
    Form,
    Table,
    Matrix,
    Image,
    Equation,
    Code,
    Button,
    Input,
    Select,
    Checkbox,
    Slider,
    Toggle,
    CellGrid,
    Tree,
    Dialog
};
NLOHMANN_JSON_SERIALIZE_ENUM(VisualNodeKind, {
    {VisualNodeKind::Text,     "text"},
    {VisualNodeKind::Panel,    "panel"},
    {VisualNodeKind::Shape,    "shape"},
    {VisualNodeKind::Chart,    "chart"},
    {VisualNodeKind::Plot,     "plot"},
    {VisualNodeKind::Graph,    "graph"},
    {VisualNodeKind::Legend,   "legend"},
    {VisualNodeKind::Form,     "form"},
    {VisualNodeKind::Table,    "table"},
    {VisualNodeKind::Matrix,   "matrix"},
    {VisualNodeKind::Image,    "image"},
    {VisualNodeKind::Equation, "equation"},
    {VisualNodeKind::Code,     "code"},
    {VisualNodeKind::Button,   "button"},
    {VisualNodeKind::Input,    "input"},
    {VisualNodeKind::Select,   "select"},
    {VisualNodeKind::Checkbox, "checkbox"},
    {VisualNodeKind::Slider,   "slider"},
    {VisualNodeKind::Toggle,   "toggle"},
    {VisualNodeKind::CellGrid, "cell_grid"},
    {VisualNodeKind::Tree,     "tree"},
    {VisualNodeKind::Dialog,   "dialog"},
})

// Authored 2D shape vocabulary. Coordinates are intentionally expressed by
// the renderer from the shape spec; VisualDocument never stores a transform.
enum class VisualShapeKind
{
    Rectangle,
    RoundedRectangle,
    Circle,
    Ellipse,
    Diamond,
    Hexagon,
    Triangle,
    Pill,
    Line,
    Polygon,
    Grid
};
NLOHMANN_JSON_SERIALIZE_ENUM(VisualShapeKind, {
    {VisualShapeKind::Rectangle,        "rectangle"},
    {VisualShapeKind::RoundedRectangle, "rounded_rectangle"},
    {VisualShapeKind::Circle,            "circle"},
    {VisualShapeKind::Ellipse,           "ellipse"},
    {VisualShapeKind::Diamond,           "diamond"},
    {VisualShapeKind::Hexagon,           "hexagon"},
    {VisualShapeKind::Triangle,          "triangle"},
    {VisualShapeKind::Pill,              "pill"},
    {VisualShapeKind::Line,              "line"},
    {VisualShapeKind::Polygon,           "polygon"},
    {VisualShapeKind::Grid,              "grid"},
})

struct VisualShapeSpec
{
    VisualShapeKind kind = VisualShapeKind::Rectangle;
    std::optional<float> corner_radius;
    std::optional<int> sides;
    nlohmann::json points;                       // authored 2D points for polygon/line
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualShapeSpec, kind, corner_radius, sides, points)

struct VisualGeometrySpec
{
    std::optional<float> width;
    std::optional<float> height;
    std::optional<float> radius;
    std::optional<VisualShapeSpec> shape;
    nlohmann::json points;                       // authored geometry, never resolved position
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualGeometrySpec, width, height, radius, shape, points)

struct VisualPort
{
    std::string id;
    std::string side = "east";                   // north | east | south | west
    float position = 0.5f;                        // 0..1 along the side
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualPort, id, side, position)

struct ChartAxis
{
    std::optional<std::string> label;
    std::optional<std::string> scale;         // linear | log | time | ordinal
    std::optional<std::string> field;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ChartAxis, label, scale, field)

struct ChartSeries
{
    std::string id;
    std::string field;
    std::optional<std::string> label;
    std::optional<std::string> color;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ChartSeries, id, field, label, color)

struct ChartSpec
{
    std::string type = "line";               // line | bar | area | scatter | pie
    std::optional<ChartAxis> x;
    std::optional<ChartAxis> y;
    std::vector<ChartSeries> series;
    std::optional<bool> legend;
    std::optional<bool> grid;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ChartSpec, type, x, y, series, legend, grid)

struct VisualInteraction
{
    bool clickable = false;
    bool hover = false;
    bool select = false;
    bool focusable = false;
    bool draggable = false;
    bool editable = false;
    bool inspect = false;
    bool activate = false;
    std::vector<std::string> actions;         // authored action ids/capabilities
    // Short capability names are the canonical v1 names. The longer fields
    // above remain part of the original authored contract.
    bool focus = false;
    bool drag = false;
    bool edit = false;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualInteraction, clickable, hover, select, focusable,
    draggable, editable, inspect, activate, actions, focus, drag, edit)

struct VisualSemanticMetadata
{
    std::optional<std::string> role;
    std::optional<std::string> concept_id;
    std::optional<std::string> kind;
    std::optional<std::string> explanation;
    std::vector<std::string> tags;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualSemanticMetadata, role, concept_id, kind, explanation, tags)

struct VisualStyleIntent
{
    std::string emphasis = "default";          // subtle | default | primary | prominent
    float opacity = 1.0f;
    bool visible = true;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualStyleIntent, emphasis, opacity, visible)

struct VisualDataBinding
{
    std::string source;                        // data source id
    std::optional<std::string> path;          // source-relative path
    std::optional<std::string> transform;     // named, renderer-defined transform
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualDataBinding, source, path, transform)

struct DataSource
{
    std::string id;
    std::string kind;                          // inline | uri | query | generated
    std::optional<std::string> uri;
    nlohmann::json value;
    nlohmann::json config;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DataSource, id, kind, uri, value, config)

struct VisualNode
{
    std::string id;
    VisualNodeKind kind = VisualNodeKind::Text;
    std::optional<std::string> label;
    std::optional<VisualLayout> layout;
    nlohmann::json content;                    // kind-specific authored content
    std::optional<VisualShapeSpec> shape;
    std::optional<VisualGeometrySpec> geometry;
    std::optional<ChartSpec> chart;
    std::optional<VisualDataBinding> binding;
    std::optional<VisualSemanticMetadata> semantic;
    std::optional<VisualInteraction> interaction;
    VisualStyleIntent style;
    std::vector<VisualPort> ports;
    std::vector<VisualNode> children;
    // DOM pointer-events:none — never hit-tested for input even when
    // interaction flags are set (children are unaffected).
    bool pass_through = false;
    /// Explicit background color (hex, e.g. "#1e293b"). Containers paint a
    /// mesh ONLY when this is set; background-less sections are transparent
    /// (DOM block background semantics) and let the world behind show through.
    std::optional<std::string> background;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualNode, id, kind, label, layout, content, shape,
    geometry, chart, binding, semantic, interaction, style, ports, children, pass_through)

struct VisualSection
{
    std::string id;
    std::optional<std::string> title;
    std::optional<VisualLayout> layout;
    std::vector<VisualNode> nodes;
    std::vector<VisualSection> sections;
    // DOM pointer-events:none — never hit-tested for input even when
    // interaction flags are set (children are unaffected).
    bool pass_through = false;
    /// Explicit background color (hex, e.g. "#1e293b"). Containers paint a
    /// mesh ONLY when this is set; background-less sections are transparent
    /// (DOM block background semantics) and let the world behind show through.
    std::optional<std::string> background;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualSection, id, title, layout, nodes, sections, pass_through, background)

struct VisualRelationStyle
{
    std::string type = "arrow";                 // line | arrow | elbow | bezier
    std::optional<std::string> color;
    std::optional<float> width;
    std::optional<bool> dashed;
    std::optional<std::string> routing;         // "grid": elbows bend on canvas grid lines
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualRelationStyle, type, color, width, dashed, routing)

struct VisualRelation
{
    std::string id;
    std::string source;
    std::string target;
    std::optional<std::string> source_port;
    std::optional<std::string> target_port;
    std::string kind;                          // links | depends_on | filters | controls
    std::optional<std::string> label;
    std::optional<VisualRelationStyle> style;
    nlohmann::json config;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualRelation, id, source, target, source_port,
    target_port, kind, label, style, config)

struct VisualDocument
{
    std::string version = "1";
    std::string id;
    VisualMetadata metadata;
    VisualCanvas canvas;
    std::string density = "standard";          // spacious | standard | dense | reference | poster
    std::optional<VisualStyleIntent> style;
    std::optional<std::string> composition;     // authored macro-strategy id
    std::vector<VisualSection> sections;
    std::vector<VisualNode> nodes;
    std::vector<VisualRelation> relations;
    std::vector<DataSource> data_sources;
    nlohmann::json initial_state;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualDocument, version, id, metadata, canvas, density,
    style, composition, sections, nodes, relations, data_sources, initial_state)

} // namespace exd
