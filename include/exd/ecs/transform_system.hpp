#pragma once

#include <exd/ecs/scene_graph.hpp>
#include <exd/ecs/system.hpp>
#include <exd/ecs/transform.hpp>

#include <unordered_set>

namespace exd::ecs {

/// Derives WorldTransform state from SceneGraph relationships + LocalTransform.
///
/// For every entity carrying a LocalTransform component, TransformSystem
/// walks the SceneGraph parent chain and accumulates:
///
///     world.position = parent_world.position + parent_world.rotation *
///                      (parent_world.scale * local.position)
///     world.rotation = parent_world.rotation * local.rotation
///     world.scale    = parent_world.scale * local.scale
///
/// Entities without LocalTransform are left untouched (they keep whatever
/// transform state their owning system manages). Parents without a
/// LocalTransform contribute identity. The SceneGraph is the only hierarchy
/// consulted — renderer-local hierarchy components are not part of this
/// system.
class TransformSystem final : public ISystem {
public:
    explicit TransformSystem(SceneGraph& scene) : scene_(scene) {}

    void update(Registry& registry, double dt) override;

    /// Compute the derived world transform for a single entity (static form,
    /// useful for systems that need an immediate value outside the update).
    [[nodiscard]] static WorldTransform derive(const Registry& registry,
                                               const SceneGraph& scene,
                                               Entity entity);

    [[nodiscard]] const SceneGraph& scene() const noexcept { return scene_; }

private:
    SceneGraph& scene_;
};

} // namespace exd::ecs
