#pragma once

#include <exd/math/quat.hpp>
#include <exd/math/vec3.hpp>

namespace exd::ecs {

/// Transform of an entity relative to its parent.
///
/// LocalTransform is plain component state: it carries no hierarchy semantics
/// by itself. Systems that need spatial hierarchy combine SceneGraph
/// relationships with LocalTransform and parent transforms to derive a
/// WorldTransform (see TransformSystem in transform_system.hpp).
///
/// The SceneGraph itself knows nothing about LocalTransform — it remains
/// useful for non-spatial hierarchies.
struct LocalTransform {
    math::Vec3f position{0.0f, 0.0f, 0.0f};
    math::Quat  rotation{1.0f, 0.0f, 0.0f, 0.0f};
    math::Vec3f scale{1.0f, 1.0f, 1.0f};
};

/// Derived, consumed transform state: the accumulated local→world transform
/// of an entity, computed by TransformSystem from SceneGraph relationships
/// and LocalTransform components. Renderers and picking systems should
/// consume WorldTransform rather than recomposing parent chains themselves.
struct WorldTransform {
    math::Vec3f position{0.0f, 0.0f, 0.0f};
    math::Quat  rotation{1.0f, 0.0f, 0.0f, 0.0f};
    math::Vec3f scale{1.0f, 1.0f, 1.0f};
};

} // namespace exd::ecs
