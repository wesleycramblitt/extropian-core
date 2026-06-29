#pragma once

/// Root namespace for all Extropian libraries.
namespace exd {}

// Sub-namespace aliases (each library uses its own)
namespace exd::math {}    // Vec3, Mat4, Quat, etc.
namespace exd::ecs {}     // Entity, Registry, ISystem, Component
namespace exd::geom {}    // Primitives, CSG, spatial queries
namespace exd::core {}    // Allocators, serialization, plugins, events, config, logging, units
