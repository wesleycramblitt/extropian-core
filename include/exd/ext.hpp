#pragma once

// Umbrella header for extropian-core.
// Pulls in all core library headers.

#include <exd/core/namespace.hpp>
#include <exd/core/units.hpp>
#include <exd/core/config.hpp>
#include <exd/core/logging.hpp>
#include <exd/core/serialization.hpp>
#include <exd/core/allocator.hpp>
#include <exd/core/plugin.hpp>
#include <exd/core/event.hpp>

#include <exd/math/vec3.hpp>
#include <exd/math/vec4.hpp>
#include <exd/math/mat3.hpp>
#include <exd/math/mat4.hpp>
#include <exd/math/quat.hpp>

#include <exd/ecs/entity.hpp>
#include <exd/ecs/component.hpp>
#include <exd/ecs/registry.hpp>
#include <exd/ecs/system.hpp>
#include <exd/ecs/view.hpp>

#include <exd/geom/primitives.hpp>
#include <exd/geom/csg.hpp>
#include <exd/geom/spatial.hpp>
