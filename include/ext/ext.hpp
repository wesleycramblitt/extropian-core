#pragma once

// Umbrella header for extropian-core.
// Pulls in all core library headers.

#include <ext/core/namespace.hpp>
#include <ext/core/units.hpp>
#include <ext/core/config.hpp>
#include <ext/core/logging.hpp>
#include <ext/core/serialization.hpp>
#include <ext/core/allocator.hpp>
#include <ext/core/plugin.hpp>
#include <ext/core/event.hpp>

#include <ext/math/vec3.hpp>
#include <ext/math/vec4.hpp>
#include <ext/math/mat3.hpp>
#include <ext/math/mat4.hpp>
#include <ext/math/quat.hpp>

#include <ext/ecs/entity.hpp>
#include <ext/ecs/component.hpp>
#include <ext/ecs/registry.hpp>
#include <ext/ecs/system.hpp>
#include <ext/ecs/view.hpp>

#include <ext/geom/primitives.hpp>
#include <ext/geom/csg.hpp>
#include <ext/geom/spatial.hpp>
