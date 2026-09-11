#pragma once

// Compatibility header. HierarchyIndex was renamed SceneGraph as part of the
// ecosystem architecture refactor: the shared, renderer-agnostic entity
// hierarchy now lives in exd::ecs::SceneGraph (see scene_graph.hpp).
//
// @deprecated — use exd::ecs::SceneGraph directly. New code should not use
// this alias; existing callers are migrated incrementally.

#include <exd/ecs/scene_graph.hpp>

namespace exd::ecs {

using HierarchyIndex = SceneGraph;

} // namespace exd::ecs
