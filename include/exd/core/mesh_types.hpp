#pragma once

#include <exd/math/vec3.hpp>
#include <exd/math/quat.hpp>
#include <vector>
#include <cstdint>

namespace exd::core {

// ── Vertex ───────────────────────────────────────

/// Canonical interleaved vertex layout used across the ecosystem.
struct Vertex {
    math::Vec3f position = {};
    math::Vec3f normal   = {0, 1, 0};
    math::Vec3f uv       = {};
    math::Quat  tangent  = {1, 0, 0, 1};
    // White = identity: shaders multiply v_color by the material color, so a
    // mesh that carries no per-vertex color must default to white (1,1,1,1).
    // The old 0.8 gray default silently darkened every mesh that never set
    // color (all CAE importers, e.g. OBJ/FBX/STL/STEP) by 20%.
    math::Quat  color    = {1.0f, 1.0f, 1.0f, 1.0f};
};

// ── Topology ─────────────────────────────────────

/// Indexed in order so that Triangles/Lines/Points values
/// match the legacy exd::render::Topology enum for GL compatibility.
enum class PrimitiveTopology {
    Triangles      = 0,
    Lines          = 1,
    Points         = 2,
    LineStrip      = 3,
    TriangleStrip  = 4
};

// ── Bounds ───────────────────────────────────────

struct Bounds {
    math::Vec3f min = {};
    math::Vec3f max = {};
};

// ── MeshData ─────────────────────────────────────

/// CPU-side mesh data.  Renderers map this to GPU buffers.
struct MeshData {
    std::vector<Vertex>    vertices;
    std::vector<uint32_t>  indices;
    PrimitiveTopology      topology = PrimitiveTopology::Triangles;
    Bounds                 bounds;
};

} // namespace exd::core
