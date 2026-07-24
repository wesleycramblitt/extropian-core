#pragma once

// Umbrella header for extropian-core.
// Pulls in all core library headers.

#include <exd/core/namespace.hpp>
#include <exd/core/assert.hpp>
#include <exd/core/clock.hpp>
#include <exd/core/config.hpp>
#include <exd/core/fs.hpp>
#include <exd/core/handle.hpp>
#include <exd/core/hash.hpp>
#include <exd/core/logging.hpp>
#include <exd/core/macros.hpp>
#include <exd/core/pool.hpp>
#include <exd/core/random.hpp>
#include <exd/core/string_util.hpp>

#include <exd/math/math.hpp>

#include <exd/ecs/component.hpp>
#include <exd/ecs/command_buffer.hpp>
#include <exd/ecs/registry.hpp>
#include <exd/ecs/system.hpp>
#include <exd/ecs/system_graph.hpp>
