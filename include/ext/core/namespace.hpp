#pragma once

/// Root namespace for all Extropian libraries.
namespace ext {}

// Sub-namespace aliases (each library uses its own)
namespace ext::math {}    // Vec3, Mat4, Quat, etc.
namespace ext::ecs {}     // Entity, Registry, ISystem, Component
namespace ext::geom {}    // Primitives, CSG, spatial queries
namespace ext::core {}    // Allocators, serialization, plugins, events, config, logging, units
