#pragma once

// Umbrella header for extropian-core.
// Pulls in all public core library headers.

// ── types ──────────────────────────────────────────
#include <exd/types/visual_intent.hpp>
#include <exd/types/semantic_document.hpp>
#include <exd/types/visual_intent_document.hpp>
#include <exd/types/presentation_state.hpp>
#include <exd/types/scene_document.hpp>

// ── core ───────────────────────────────────────────
#include <exd/core/namespace.hpp>
#include <exd/core/allocator.hpp>
#include <exd/core/assert.hpp>
#include <exd/core/clock.hpp>
#include <exd/core/config.hpp>
#include <exd/core/fs.hpp>
#include <exd/core/handle.hpp>
#include <exd/core/hash.hpp>
#include <exd/core/logging.hpp>
#include <exd/core/macros.hpp>
#include <exd/core/mesh_types.hpp>
#include <exd/core/pool.hpp>
#include <exd/core/random.hpp>
#include <exd/core/serialization.hpp>
#include <exd/core/string_util.hpp>
#include <exd/core/visual_tree.hpp>
#include <exd/core/units.hpp>
#include <exd/core/window_state.hpp>

// ── math ───────────────────────────────────────────
#include <exd/math/math.hpp>

// ── ecs ────────────────────────────────────────────
#include <exd/ecs/component.hpp>
#include <exd/ecs/entity.hpp>
#include <exd/ecs/command_buffer.hpp>
#include <exd/ecs/registry.hpp>
#include <exd/ecs/view.hpp>
#include <exd/ecs/system.hpp>
#include <exd/ecs/system_graph.hpp>
